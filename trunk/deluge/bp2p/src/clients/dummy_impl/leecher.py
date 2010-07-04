from betterp2p.network.simple import SimpleNetwork
from betterp2p.network.simple.primitives import ByeCommand, InterestedCommand
import logging


class Leecher(object):
    
    DEFAULT_SEEDER = ('127.0.0.1', 9876)
    
    def __init__(self, target=None):
        self.network = SimpleNetwork({'port': 9878})
        self.target = target or self.DEFAULT_SEEDER
    
    def main(self):
        logging.root.setLevel(logging.INFO)
        logging.getLogger("bp2p.net").setLevel(logging.INFO)
        logging.info("Leecher")
        logging.info("-- connecting to %s:%d" % self.target)
        r = self.network.conn_mgr.connect_to(self.target)
        def request_chunk(peer):
            q = self.network.conn_mgr.request_chunk(peer, file_id="shared1")
            q.addCallback(lambda x: send_bye(peer)) 
        def send_bye(peer):
            peer.sendCommand(ByeCommand())
        def connection_failed(reason):
            logging.root.fatal("Leecher: failed connecting to Seeder.")
            self.network.conn_mgr.terminate()
        r.addCallback(request_chunk)
        r.addErrback(connection_failed)
        self.network.conn_mgr.serve_forever()


if __name__ == '__main__':
    import sys
    if len(sys.argv) == 1:
        l = Leecher()
    elif len(sys.argv) == 2:
        l = Leecher((sys.argv[1], Leecher.DEFAULT_SEEDER[1]))
    elif len(sys.argv) == 3:
        l = Leecher((sys.argv[1], int(sys.argv[2])))
    else:
        print "Usage: leecher.py [seeder-host [seeder-port]]"
        sys.exit(1)

    l.main()
