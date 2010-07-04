from twisted.internet.protocol import Protocol, Factory, ClientFactory
from twisted.internet import reactor, defer
from twisted.internet.address import IPv4Address
from twisted.protocols.basic import LineReceiver
import logging
import primitives
import transit


log = logging.getLogger("bp2p.net.connmgr")


class PeerControlProtocol(LineReceiver):

    CHUNK_PAYLOAD_SIZE = (2<<20)

    def __init__(self):
        self.command_queue = []
        self.handshake = defer.Deferred()
        
    def connectionMade(self):
        self.sendCommand(primitives.HelloCommand(alias=self.factory.alias))
        self.factory.register_peer(self)

    def connectionLost(self, reason):
        self.factory.unregister_peer(self, reason)

    def lineReceived(self, data):
        try:
            command = primitives.Command.parse(data)
            log.info("-> %r" % command)
            if command.NAME != "OK":
                self.sendCommand(primitives.OkCommand())
            if command.NAME == "HELO":
                self.handshake.callback(self)
            elif command.NAME == "INTERESTED":
                address = IPv4Address('TCP',
                                      self.transport.getPeer().host,
                                      command.transit_port)
                try:
                    self.supply(address, command.file_id)
                except:
                    import traceback, sys
                    traceback.print_exc()
                    self.sendCommand(primitives.ByeCommand())
            elif command.NAME == "BYE":
                self.transport.loseConnection()
        except primitives.Command.ParseError, e:
            log.info("-> " + data + " --- invalid")
            self.transport.write("ERROR %s\n" % str(e).replace(" ", "_"))
    
    def supply(self, peer_address, file_id):
        """
        Sends a chunk to an interested peer.
        @param peer_address an IPv4Address of peer
        @param file_id file to send a chunk from
        """
        if self.factory.share_manager:
            peer_details = (peer_address.host, peer_address.port)
            (chunk_header, chunk_data) = self.factory.share_manager.supply_chunk(peer_details, file_id)
            put = transit.PeerTransitPutProtocolFactory(file_id, 0, len(chunk_header), chunk_header + chunk_data)
            reactor.connectTCP(peer_address.host, peer_address.port, put)

    def sendCommand(self, cmd):
        log.info("<- %r" % cmd)
        self.sendLine(str(cmd))


class PeerControlProtocolFactory(ClientFactory):

    protocol = PeerControlProtocol
    alias = "bp2p"
    
    def __init__(self):
        self.remotes = { }
        self.pending = []
        self.chunk_supplier = None
    
    def clientConnectionFailed(self, connector, reason):
        address = connector.getDestination()
        self.give_up_peer(address, reason)
    
    def register_peer(self, peer):
        """
        @param peer a PeerControlProtocol instance
        """
        address = peer.transport.getPeer()
        log.info("connected %s" % address)
        key = (address.host, address.port)
        for (k, p) in self.pending:
            if k == key:
                p.callback(peer)
        self.pending = [(k,p) for (k,p) in self.pending if k != key]
        self.remotes[key] = peer
    
    def give_up_peer(self, address, reason):
        log.error("failed connecting to %s:%d (%s)" % \
                  (address.host, address.port, reason.getErrorMessage()))
        key = (address.host, address.port)
        for (k, p) in self.pending:
            if k == key:
                p.errback(reason)
        self.pending = [(k,p) for (k,p) in self.pending if k != key]
    
    def unregister_peer(self, peer, reason):
        address = peer.transport.getPeer()
        log.info("disconnected %s (%s)" % (address, reason.getErrorMessage()))
    
    def get_peer(self, (host, port)):
        key = (host, port)
        try:
            return defer.succeed(self.remotes[key])
        except KeyError:
            n = defer.Deferred()
            self.pending.append((key, n))
            return n
        


class ConnectionManager(object):
    
    DEFAULT_PORT = 9876
    
    def __init__(self, share_manager, port=None):
        self.control = PeerControlProtocolFactory()
        self.control.port = (port or self.DEFAULT_PORT)
        self.control.share_manager = share_manager

        self.transit = transit.PeerTransitGetProtocolFactory()
        self.transit.port = self.control.port + 1
         
        reactor.listenTCP(self.control.port, self.control)
        reactor.listenTCP(self.transit.port, self.transit)

    def connect_to(self, (host, port)):
        def do_connect():
            reactor.callLater(0, lambda: reactor.connectTCP(host, port, self.control))
        reactor.callFromThread(do_connect)
        return self.control.get_peer((host, port))

    def request_chunk(self, peer, file_id):
        req = self.transit.request(file_id)
        cmd = primitives.InterestedCommand(file_id=file_id, 
                                           transit_port=self.transit.port)
        peer.sendCommand(cmd)
        return req

    def serve_forever(self):
        log.info("Started; port=%d" % (self.control.port))
        reactor.run()

    def terminate(self):
        try:
            reactor.stop()
        except:
            log.warn("couldn't stop reactor")
            
    def from_thread(self, func, args):
        reactor.callFromThread(func, *args)
