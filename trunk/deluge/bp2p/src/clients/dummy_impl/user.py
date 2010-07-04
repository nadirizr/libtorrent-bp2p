from betterp2p.network.simple import SimpleNetwork
from betterp2p.network.simple.share import Share, Peer
import logging
import thread
import code


class User(object):
    """
    A Simple network client with an interactive user console.
    """
    
    def __init__(self, network_config={ }):
        self.network = SimpleNetwork(network_config)
    
    def main(self):
        logging.root.setLevel(logging.INFO)
        logging.getLogger("bp2p.net").setLevel(logging.INFO)
        logging.info("User; entering interactive mode")
        thread.start_new(self.interaction, ())
        self.network.mainloop()
        
    def interaction(self):
        code.interact(banner="BetterP2P console", 
                       local={'u': self,
                              'n': self.network,
                              'Share': Share,
                              'Peer': Peer})
        self.network.conn_mgr.from_thread(self.network.conn_mgr.terminate, ())
    
    def get_file1(self):
        """
        For example, try calling this:
        >>> u.get_file1()
        """
        file1 = Share("file1")
        self.network.create_share(file1)
        self.network.connect_peer(file1, Peer(('localhost', 9876)))


if __name__ == '__main__':
    import getopt, sys
    try:
        opts, args = getopt.getopt(sys.argv[1:], "p:")
    except getopt.GetoptError, e:
        print "invalid arguments:", e
        sys.exit(1)
    if args:
        print "unrecognized args:", args
        sys.exit(1)
    
    network_config = { }
    for flag, value in opts:
        if flag == "-p":
            try:
                network_config["port"] = int(value)
            except ValueError:
                print "invalid port number:", value
    
    u = User(network_config)
    u.main()
