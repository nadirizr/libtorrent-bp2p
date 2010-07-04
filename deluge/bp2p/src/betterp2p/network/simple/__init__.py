"""Simple Network Layer

A basic text-based protocol for transferring chunks between peers,
mainly for testing, benchmark, and demonstration purposes.
"""

from connection_manager import ConnectionManager
import share
import logging

log = logging.getLogger("bp2p.net")

class SimpleNetwork(object):
    """
    Main entry point for Simple network protocol implementation.
    """
    
    def __init__(self, share_manager=None, config={ }):
        self.config = config.copy()
        self.shares = { }
        self.share_manager = share_manager
        self.conn_mgr = ConnectionManager(self, port=config.get("port", None))
    
    def mainloop(self):
        self.conn_mgr.serve_forever()

    def stop_mainloop(self):
        self.conn_mgr.terminate()
    
    def create_share(self, share):
        self.shares[share.file_id] = share
        
    def connect_peer(self, share, peer):
        host, port = peer.address
        r = self.conn_mgr.connect_to((host, port))
        r.addCallback(lambda connection: self._connect_peer_done(share, peer, 
                                                                 connection))
        r.addErrback(lambda reason: self._connect_peer_err(peer, reason))
    
    def request_chunk(self, share, peer):
        q = self.conn_mgr.request_chunk(peer.connection, share.file_id)
        q.addCallback(lambda data: self._request_chunk_done(share, peer, data))
        q.addErrback(lambda failure: self._request_chunk_err(share, peer, failure))
        return q

    def supply_chunk(self, peer_address, file_id):
        if file_id not in self.shares:
            raise RuntimeError("not sharing this file: %s" % file_id)

        share = self.shares[file_id]
        peers = [p for p in share.peers if p.address == peer_address]
        peer = None
        if peers: peer = peers[0]
        
        return self.share_manager.supply_chunk(share, peer)  

    def _connect_peer_done(self, share, peer, connection):
        peer.connection = connection
        share.peers.append(peer)
        if share.downloading:
            self.request_chunk(share, peer)
    
    def _connect_peer_err(self, peer, failure):
        host, port = peer.address
        log.info("Failed connecting to %s:%d [%s]" % (host, port, failure))
            
    def _request_chunk_done(self, share, peer, res):
        host, port = peer.address
        log.info("Chunk expected; file_id='%s', peer=%s:%d, datalen=%d" % \
                 (share.file_id, host, port, len(res.data)))
        log.info("Chunk received; file_id='%s', peer=%s:%d, datalen=%d" % \
                 (res.file_id, host, port, len(res.data)))
        log.info("self.shares['%s'].file_id = '%s'" % (res.file_id, self.shares[res.file_id].file_id))
        log.info("share.file_id = '%s'" % share.file_id)
        self.share_manager.chunk_done(share, peer, res)

    def _request_chunk_err(self, share, peer, reason):
        host, port = peer.address
        log.info("Chunk failed; file_id='%s', peer=%s:%d, reason=%s" % \
                 (share.file_id, host, port, reason))
