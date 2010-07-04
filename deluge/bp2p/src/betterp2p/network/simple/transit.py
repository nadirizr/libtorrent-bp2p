from twisted.internet.protocol import Protocol, Factory, ClientFactory
from twisted.internet import reactor, defer
from twisted.protocols.basic import LineReceiver
import primitives
import logging
import threading


log = logging.getLogger("bp2p.net.transit")



class PeerTransitGetProtocol(LineReceiver):
    """
    The receiving side of a chunk exchange.
    """
    
    def __init__(self):
        self.file_id = None
        self.chunk_id = None
        self.header_size = None
        self.header = []
        self.data_size = None
        self.data = []
    
    def lineReceived(self, data):
        log.info("received line '%s'" % data)
        try:
            command = primitives.Command.parse(data)
            log.info("-> %s" % (command,))
            if command.NAME == "CHUNK":
                self.file_id = command.file_id
                self.chunk_id = command.chunk_id
                self.header_size = command.header_size
                self.data_size = command.size
                self.setRawMode()
        except primitives.Command.ParseError, e:
            log.info("-> %s --- invalid: %s" % (data, e))
            self.transport.loseConnection()

    def rawDataReceived(self, data):
        log.info("received %d bytes." % len(data))
        self.data.extend(data)
        print "/// *** rawDataReceived *** ///"
        print len(data), len(self.data)
        if self.data_size is not None:
            percent = len(self.data) * 100 / self.data_size
            log.info(" -- %d%% of chunk" % percent)
    
    def connectionLost(self, reason):
        print "/// *** connectionLost *** ///"
        print reason, len(self.data), self.data_size
        # final touchups and data processing
        if len(self.data) == self.data_size:
            print >> file("/tmp/leecher", "a"), self.file_id
            self.header = "".join(self.data[:self.header_size])
            self.data = "".join(self.data[self.header_size:])
            self.factory.result(self)
        else:
            address = self.transport.getPeer()
            log.error("transit from %s ended (%s)" % (address, reason.getErrorMessage()))

class PeerTransitGetProtocolFactory(Factory):
    """
    Manages incoming chunks and routes them to objects which
    requested them.
    """

    protocol = PeerTransitGetProtocol

    class ShareHandler:
        def __init__(self, file_id):
            self.file_id = file_id
            self.requests = []
            self.results = []
            print "$$$ created ShareHandler for file '%s' (req:%d,res:%d)" % (file_id, len(self.requests), len(self.results))

    def __init__(self):
        self.share_handlers = {}
        self.semaphore = threading.Lock()
    
    def request(self, file_id):
        print "^^^ request: requested chunk for file '%s'" % file_id
        self.semaphore.acquire()
        n = defer.Deferred()
        self._get_handler(file_id).requests.insert(0, n)
        self.semaphore.release()
        return n
    
    def result(self, res):
        self.semaphore.acquire()
        print "^^^ result: received result for file '%s'" % res.file_id
        print "^^^ --> sh.file_id = '%s'" % self._get_handler(res.file_id).file_id
        self._get_handler(res.file_id).results.insert(0, res)
        self.semaphore.release()
        self._flush_results()
        
    def _get_handler(self, file_id):
        if file_id not in self.share_handlers:
            print "$$$ '%s' not found in self.share_handlers - creating..." % file_id
            self.share_handlers[file_id] = PeerTransitGetProtocolFactory.ShareHandler(file_id)
        return self.share_handlers[file_id]

    def _flush_results(self):
        self.semaphore.acquire()
        print "&&& _flush_results (len(self.shares) = %d):" % (len(self.share_handlers))
        for s in self.share_handlers:
            ss = self.share_handlers[s]
            print "&&& --> '%s' (req:%d,res:%d)" % (s,len(ss.requests),len(ss.results))
        for s in self.share_handlers.values():
            print "&&& checking '%s' (req:%d,res:%d)" % (s.file_id,len(s.requests),len(s.results))
            while s.results and s.requests:
                print "&&& --> flushing one request..."
                req = s.requests.pop()
                res = s.results.pop()
                self.semaphore.release()
                req.callback(res)
                self.semaphore.acquire()
        print "&&& _flush_results done!"
        self.semaphore.release()


class PeerTransitPutProtocol(LineReceiver):
    """
    The sending side of a chunk exchange.
    """
    
    def connectionMade(self):
        address = self.transport.getPeer()
        log.info("transit of '%s' to %s started" % (self.factory.file_id, address))
        self.sendCommand(primitives.ChunkCommand(file_id=self.factory.file_id,
                                                 chunk_id=self.factory.chunk_id,
                                                 header_size=self.factory.header_size,
                                                 size=len(self.factory.payload)))
        log.info("sending %d bytes." % len(self.factory.payload))
        print "/// *** connectionMade *** ///"
        self.transport.write(self.factory.payload)
        self.transport.loseConnection()
        
    def connectionLost(self, reason):
        address = self.transport.getPeer()
        log.info("transit to %s ended (%s)" % (address, reason.getErrorMessage()))

    def sendCommand(self, cmd):
        log.info("<- %r" % cmd)
        self.sendLine(str(cmd))


class PeerTransitPutProtocolFactory(ClientFactory):
    protocol = PeerTransitPutProtocol

    def __init__(self, file_id, chunk_id, header_size, payload):
        self.file_id = file_id
        self.chunk_id = chunk_id
        self.header_size = header_size
        self.payload = payload

