#!/usr/bin/python

import unittest
import os
import tempfile
import shutil
import random
import struct

from betterp2p.cbp2p import BT2
from betterp2p.control import file_controller

class SystemTests(unittest.TestCase):
    def __init__(self, *args, **kwargs):
        super(SystemTests, self).__init__(*args, **kwargs)
        self.chunk_size = 100 * 1024 # 100k

    def setUp(self):
        #import pdb
        #pdb.set_trace()
        self.temp_dir = self.createTempTestsDir()
        self.full_ctrl = self.createFullFileObject(self.createRandomFile())
        new_file = tempfile.mktemp(dir=self.temp_dir)
        new_meta = BT2.MetaData(new_file, new_file, self.chunk_size, self.full_ctrl.metadata.fileSize)
        self.new_ctrl = self.createEmptyFileObject(new_file, new_meta)
        self.final_file_set = set([self.full_ctrl.filename, self.new_ctrl.filename])

    def tearDown(self):
        shutil.rmtree(self.temp_dir)

    def testReconstructFullFile(self):
        bad_chunks = 0
        while not self.new_ctrl.is_done():
            if not self.new_ctrl.put_chunk(self.full_ctrl.get_chunk()):
                bad_chunks += 1

        self.assertTrue(self.new_ctrl.is_done(), "Matrix elimination failed")
        self.assert_(bad_chunks < 5,
                            "Too many bad chunks recieved: %d" % bad_chunks)
        self.assertEqualFiles(self.full_ctrl, self.new_ctrl)
        self.assertUnexpectedFilesInDir(self.temp_dir, self.final_file_set)

    def testReconstructStopResume(self):
        chunks_count = self.full_ctrl.metadata.getChunkCount()
        bad_chunks = 0
        for i in range(chunks_count /2):
            if not self.new_ctrl.put_chunk(self.full_ctrl.get_chunk()):
                bad_chunks += 1

        self.assert_(bad_chunks < 2, "Too many bad chunks recieved: %d" % bad_chunks)

        new_filename = self.new_ctrl.metadata.diskFileName
        del self.new_ctrl
        newer_ctrl = file_controller.FileController(new_filename)
        
        bad_chunks = 0
        while not newer_ctrl.is_done():
            if not newer_ctrl.put_chunk(self.full_ctrl.get_chunk()):
                bad_chunks += 1

        self.assertTrue(newer_ctrl.is_done(), "Matrix elimination failed")
        self.assert_(bad_chunks < 5,
                            "Too many bad chunks recieved: %d" % bad_chunks)
        self.assertEqualFiles(self.full_ctrl, newer_ctrl)
        self.assertUnexpectedFilesInDir(self.temp_dir, self.final_file_set)

    def testReconstructFromPartialFiles(self):
        partial_files = [tempfile.mktemp(dir=self.temp_dir) for x in range(3)]
        partial_ctrls = [self.createEmptyFileObject(f,
                         BT2.MetaData(f, f, self.chunk_size, self.full_ctrl.metadata.fileSize))
                         for f in partial_files]
        self.final_file_set.update(pc.filename for pc in partial_ctrls)
        #self.final_file_set.update(pc.filename + ".meta" for pc in partial_ctrls)
        # the .meta files are now in a separate dir

        chunks_count = self.full_ctrl.metadata.getChunkCount()

        bad_chunks = 0
        for pc in partial_ctrls:
            for i in range(chunks_count / 2):
                chunk = self.full_ctrl.get_chunk()
                if not pc.put_chunk(chunk):
                    bad_chunks += 1

        self.assert_(bad_chunks < len(partial_ctrls),
                     "Too many bad chunks recieved: %d" % bad_chunks)

        bad_chunks = 0
        for i in range(chunks_count / 2 + 1):
            for pc in partial_ctrls:
                chunk = pc.get_chunk()
                if not self.new_ctrl.put_chunk(chunk):
                    bad_chunks += 1
                if self.new_ctrl.is_done():
                    break

        self.assertTrue(self.new_ctrl.is_done(),
            "Failed to recreate the final file (bad chunks count: %d)" % bad_chunks)
        self.assert_(bad_chunks < 10, "Too many bad chunks recieved: %d" % bad_chunks)
        self.assertEqualFiles(self.full_ctrl, self.new_ctrl)
        self.assertUnexpectedFilesInDir(self.temp_dir, self.final_file_set)

    def assertEqualFiles(self, orig_ctrl, new_ctrl):
      orig_data = open(orig_ctrl.filename).read()
      new_data = open(new_ctrl.filename).read()
      self.assertEquals(orig_data, new_data[:len(orig_data)],
                        "Files are not equal (at all)")
      self.assertEquals(orig_data, new_data,
                        "New file is (probably) not truncated")

    def assertUnexpectedFilesInDir(self, dir, files_set):
        files_in_dir = set([os.path.join(dir, f) for f in os.listdir(dir)])
        self.assertEquals(files_set, files_in_dir,
                "Unexpected file in dir: %s." % (files_in_dir - files_in_dir))

    def createEmptyFileObject(self, fname, metadata):
        return file_controller.FileController(metadata, "NEW")

    def createFullFileObject(self, fname):
        print "// fname = %s" % fname
        metadata = BT2.MetaData(fname, fname, self.chunk_size)
        return file_controller.FileController(metadata, "COMPLETE")

    def createTempTestsDir(self):
        temp_dir = tempfile.mktemp()
        os.makedirs(temp_dir)
        return temp_dir

    def createRandomString(self, size):
        bytes_in_num = struct.calcsize("Q")
        list = (random.getrandbits(bytes_in_num * 8) for i in xrange(size/8))
        return struct.pack("%dQ" % (size / bytes_in_num), *list)

    def createRandomFile(self, size=1024*1024):
        file_data = self.createRandomString(size)
        file_name = tempfile.mktemp(dir=self.temp_dir)
        open(file_name, "w").write(file_data)
        return file_name

if __name__ == "__main__":
    unittest.main()
