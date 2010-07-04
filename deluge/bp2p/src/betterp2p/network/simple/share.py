
class Share(object):
    
    def __init__(self, file_id, file_control = None):
        self.file_id = file_id
        self.seeding = False
        self.downloading = False
        self.peers = []
        self.file_control = file_control


class Peer(object):
    
    def __init__(self, (host, ip)):
        self.address = (host, ip)
        self.last_recv_timestamp = 0
        self.connection = None
