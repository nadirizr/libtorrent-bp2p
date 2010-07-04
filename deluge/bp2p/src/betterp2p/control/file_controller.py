from betterp2p.cbp2p import BT2
from robin import disown

import struct

class FileController:
    """
    This is the main class that handles a single torrent file and the algorithms
    used to process it.
    This is the wrapper for the C++ code.
    """

    FILE_STATE = { 
        "UNKNOWN": BT2.FileObject.UNKNOWN,
        "NEW": BT2.FileObject.NEW, 
        "INCOMPLETE": BT2.FileObject.INCOMPLETE,
        "COMPLETE": BT2.FileObject.COMPLETE
    }

    def _init_filename(self, diskfilename):
        """
        Initializes the controller from the file name on disk.
        This assumes that there is an existing meta-file.
        """
        self.handler = BT2.FileHandler(diskfilename)

    def _init_metadata(self, metadata, filestate="UNKNOWN"):
        """
        Initializes the controller from the meta-data object, and optionally 
        the given file state.
        """
        print "// _init_metadata"
        if filestate not in self.FILE_STATE:
            raise ValueError("file-state has an invalid value (valid: '%s')" % self.FILE_STATE.keys())
        self.handler = BT2.FileHandler(metadata, self.FILE_STATE[filestate])

    def __init__(self, *args):
        """
        Creates a file handler for the given file.
        The parameters may be a filename on disk, or a meta-data file-state combination.
        """
        if len(args) == 1 and type(args[0]) == str:
            self._init_filename(*args)
        elif len(args) == 2:
            if type(args[0]) == BT2.MetaData and type(args[1]) == str:
                self._init_metadata(*args)
        elif len(args) == 1:
            if type(args[0]) == BT2.MetaData:
                self._init_metadata(*args)
        else:
            raise ValueError("given arguments are invalid - %s" % str(args))

    @property
    def metadata(self):
        return self.handler.getMetaData()

    @property
    def filename(self):
        return self.metadata.fileName

    def get_chunk(self):
        """
        Retrieves a new chunk from the underlying file.
        """
        c = self.handler.getChunk()
        return (BT2.bitvector_to_string(c.getFunctional()), c.getData())

    def put_chunk(self, chunk):
        """
        Inserts a new chunk into the underlying file.
        It returns true if the chunk contained usefull data.
        It is necessary to check if we are in endgame or we are done downloading
        after this.

        chunk is a tuple of the functionals data and the data itself.
        """
        bv = BT2.bitvector_from_string(chunk[0], self.metadata.getMaxChunkCount())
        return self.handler.putChunk(BT2.Chunk(bv, chunk[1]))

    def is_endgame(self):
        """
        Returns true if we are in endgame mode, false otherwise.
        """
        return self.handler.isEndGame()

    def is_done(self):
        """
        Returns true if we are done downloading, false otherwise.
        """
        return self.handler.isDone()

