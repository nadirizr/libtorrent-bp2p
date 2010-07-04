#! /usr/local/bin/python

from betterp2p.network.simple import SimpleNetwork
from betterp2p.network.simple.primitives import ByeCommand, InterestedCommand
from betterp2p.network.simple.share import Share, Peer
from betterp2p.control.file_controller import FileController
from betterp2p.cbp2p import BT2
from glob import glob
import logging
import os

class SimpleClient:
    """
    Performs simple seeding and leeching of files, using the simple protocol implementation.
    """

    DEFAULT_CONFIG = {"seeding_dir": "./seeding", "leeching_dir": "./leeching",
            "target_files": "./targets", "chunk_size": 10*1024,
            "port": 9876, "target_host": ("127.0.0.1", 9875)}
    
    def __init__(self, config={}):
        self.config = SimpleClient.DEFAULT_CONFIG.copy()
        self.config.update(config)

        self.peer = None
        self.network = SimpleNetwork(self, {"port": self.config["port"]})
        self._load_seeding_shares()
        self._load_leeching_shares()
        self._load_target_shares()

    def run(self):
        """
        Runs the client.
        """
        logging.root.setLevel(logging.INFO)
        logging.getLogger("bp2p.net").setLevel(logging.INFO)
        logging.info("Seedeing at port %d" % self.config["port"])
        
        # if we are leeching from some host, connect to it
        if self.config["target_host"]:
            logging.info("Leecher")
            logging.info("-- connecting to %s:%d" % (self.config["target_host"]))
            self._connect_peers()

        
        # start serving
        self.network.mainloop()

    def supply_chunk(self, share, peer):
        """
        Supplies a new chunk for the given share and requesting peer.
        """
        c = share.file_control.get_chunk()
        print "// supply_chunk:"
        print "   len(header) = %d" % len(c[0])
        print "   len(data) = %d" % len(c[1])
        print "   is_endgame = %s" % str(share.file_control.is_endgame())
        return c

    def chunk_done(self, share, peer, res):
        print "// Chunk Done!"
        print "   file_id = %s" % share.file_id
        print "   peer = (%s, %d)" % peer.address
        print "   len(data) = %d" % len(res.data)
        print "   header = %s (%d)" % (res.header, len(res.header))
        share.file_control.put_chunk((res.header, res.data))

        if share.file_control.is_done():
            share.downloading = False
            share.seeding = True
        else:
            if share.peers:
                self.network.request_chunk(share, share.peers[0])
    
    def _load_seeding_shares(self):
        """
        Loads all the seeding files in the directory.
        """
        filenames = glob("%s/*" % (self.config["seeding_dir"]))
        filenames = [os.path.basename(f) for f in filenames if not f.endswith(".meta")]
        for f in filenames:
            diskf = self.config["seeding_dir"] + os.path.sep + f
            fcl = FileController(BT2.MetaData(f, diskf, self.config["chunk_size"]), "COMPLETE")
            
            share = Share(f, fcl)
            share.downloading = False
            share.seeding = True
            self.network.create_share(share)

    def _load_leeching_shares(self):
        """
        Loads all the leeching files in the directory.
        """
        filenames = glob("%s/*" % (self.config["leeching_dir"]))
        filenames = [os.path.basename(f) for f in filenames if not f.endswith(".meta")]
        for f in filenames:
            diskf = self.config["leeching_dir"] + os.path.sep + f
            fcl = FileController(diskf)
            
            share = Share(f, fcl)
            share.downloading = True
            share.seeding = True
            self.network.create_share(share)

    def _load_target_shares(self):
        """
        Loads all the target files as shares from the target file.
        """
        if not os.path.exists(self.config["target_files"]):
            return

        filenames = file(self.config["target_files"], "r").readlines()
        sizes = [f.split()[-1] for f in filenames]
        filenames = [" ".join(f.split()[:-1]) for f in filenames]
        for i in range(len(filenames)):
            f = filenames[i]
            diskf = self.config["leeching_dir"] + os.path.sep + filenames[i]
            if os.path.exists(diskf):
                continue
            size = int(sizes[i])
            fcl = FileController(BT2.MetaData(f, diskf, self.config["chunk_size"], size), "NEW")

            share = Share(f, fcl)
            share.downloading = True
            share.seeding = False
            self.network.create_share(share)

    def _connect_peers(self):
        """
        Connects our target seeder to all of our downloading shares.
        """
        peer = Peer(self.config["target_host"])
        for share in self.network.shares.values():
            self.network.connect_peer(share, peer)


from sys import argv
# read the configuration file given as argument, or use defaults if not
config = {}
if len(argv) > 1:
    exec file(argv[1],"r") in config

cl = SimpleClient(config)
cl.run()
