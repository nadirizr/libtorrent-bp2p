<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="file">
    <name>PartialAlgorithm.h</name>
    <path>/home/nadir/Work/bp2p/current/src/betterp2p/algorithm/</path>
    <filename>PartialAlgorithm_8h</filename>
    <includes id="BitVector_8h" name="BitVector.h" local="no" imported="no">common/BitVector.h</includes>
    <includes id="FileObject_8h" name="FileObject.h" local="no" imported="no">filesystem/FileObject.h</includes>
    <class kind="class">BT2::PartialAlgorithm</class>
    <namespace>BT2</namespace>
  </compound>
  <compound kind="file">
    <name>BitVector.h</name>
    <path>/home/nadir/Work/bp2p/current/src/betterp2p/common/</path>
    <filename>BitVector_8h</filename>
    <class kind="class">BT2::BitVector</class>
    <namespace>BT2</namespace>
    <namespace>boost</namespace>
    <namespace>boost::serialization</namespace>
    <member kind="function">
      <type>BitVector</type>
      <name>bitvector_from_string</name>
      <anchorfile>namespaceBT2.html</anchorfile>
      <anchor>ec25942d9d92338be4fb202ca194a966</anchor>
      <arglist>(const std::string &amp;data, size_t bitlen=0)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>bitvector_to_string</name>
      <anchorfile>namespaceBT2.html</anchorfile>
      <anchor>9fd2fcf7e644c2139520255384484efe</anchor>
      <arglist>(const BitVector &amp;bits)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>save</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>311c699f0e0348991ee36978f7070264</anchor>
      <arglist>(Archive &amp;ar, const BT2::BitVector &amp;bv, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>2c8520eb09f488e8d87401bdec9b778e</anchor>
      <arglist>(Archive &amp;ar, BT2::BitVector &amp;bv, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialize</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>4d6f3b604885b5ee3fecbdeb526eeac4</anchor>
      <arglist>(Archive &amp;ar, BT2::BitVector &amp;t, const unsigned int file_version)</arglist>
    </member>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespaceboost.html</anchorfile>
      <anchor>f208259cc0458fe578f0e172662c6247</anchor>
      <arglist>(std::ostream &amp;os, const BT2::BitVector &amp;)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>MetaData.h</name>
    <path>/home/nadir/Work/bp2p/current/src/betterp2p/common/</path>
    <filename>MetaData_8h</filename>
    <includes id="BitVector_8h" name="BitVector.h" local="no" imported="no">common/BitVector.h</includes>
    <class kind="struct">BT2::MetaData</class>
    <class kind="struct">BT2::MetaData::FileData</class>
    <namespace>BT2</namespace>
    <namespace>boost</namespace>
    <namespace>boost::serialization</namespace>
    <member kind="function">
      <type>void</type>
      <name>save</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>66b9c2644e3181f2d2065d985eb792d6</anchor>
      <arglist>(Archive &amp;ar, const BT2::MetaData::FileData &amp;fd, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>db2d23219390759d559d9eeab3bc9100</anchor>
      <arglist>(Archive &amp;ar, BT2::MetaData::FileData &amp;fd, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialize</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>51dae346a1b15a777f49f365fe93c065</anchor>
      <arglist>(Archive &amp;ar, BT2::MetaData::FileData &amp;t, const unsigned int file_version)</arglist>
    </member>
  </compound>
  <compound kind="file">
    <name>FileHandler.h</name>
    <path>/home/nadir/Work/bp2p/current/src/betterp2p/control/</path>
    <filename>FileHandler_8h</filename>
    <includes id="MetaData_8h" name="MetaData.h" local="yes" imported="no">../common/MetaData.h</includes>
    <includes id="FileObject_8h" name="FileObject.h" local="yes" imported="no">../filesystem/FileObject.h</includes>
    <includes id="PartialAlgorithm_8h" name="PartialAlgorithm.h" local="yes" imported="no">../algorithm/PartialAlgorithm.h</includes>
    <class kind="class">BT2::FileHandler</class>
    <namespace>BT2</namespace>
  </compound>
  <compound kind="file">
    <name>Chunk.h</name>
    <path>/home/nadir/Work/bp2p/current/src/betterp2p/filesystem/</path>
    <filename>Chunk_8h</filename>
    <includes id="BitVector_8h" name="BitVector.h" local="no" imported="no">common/BitVector.h</includes>
    <class kind="class">BT2::Chunk</class>
    <namespace>BT2</namespace>
  </compound>
  <compound kind="file">
    <name>FileObject.h</name>
    <path>/home/nadir/Work/bp2p/current/src/betterp2p/filesystem/</path>
    <filename>FileObject_8h</filename>
    <includes id="Chunk_8h" name="Chunk.h" local="yes" imported="no">Chunk.h</includes>
    <includes id="MetaFile_8h" name="MetaFile.h" local="yes" imported="no">MetaFile.h</includes>
    <includes id="MetaData_8h" name="MetaData.h" local="no" imported="no">common/MetaData.h</includes>
    <class kind="class">BT2::FileObject</class>
    <namespace>BT2</namespace>
  </compound>
  <compound kind="file">
    <name>MetaFile.h</name>
    <path>/home/nadir/Work/bp2p/current/src/betterp2p/filesystem/</path>
    <filename>MetaFile_8h</filename>
    <includes id="MetaData_8h" name="MetaData.h" local="no" imported="no">common/MetaData.h</includes>
    <class kind="class">BT2::MetaFile</class>
    <namespace>BT2</namespace>
  </compound>
  <compound kind="namespace">
    <name>boost</name>
    <filename>namespaceboost.html</filename>
    <namespace>boost::serialization</namespace>
    <member kind="function">
      <type>std::ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>namespaceboost.html</anchorfile>
      <anchor>f208259cc0458fe578f0e172662c6247</anchor>
      <arglist>(std::ostream &amp;os, const BT2::BitVector &amp;)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>boost::serialization</name>
    <filename>namespaceboost_1_1serialization.html</filename>
    <member kind="function">
      <type>void</type>
      <name>save</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>66b9c2644e3181f2d2065d985eb792d6</anchor>
      <arglist>(Archive &amp;ar, const BT2::MetaData::FileData &amp;fd, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>db2d23219390759d559d9eeab3bc9100</anchor>
      <arglist>(Archive &amp;ar, BT2::MetaData::FileData &amp;fd, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialize</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>51dae346a1b15a777f49f365fe93c065</anchor>
      <arglist>(Archive &amp;ar, BT2::MetaData::FileData &amp;t, const unsigned int file_version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>save</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>311c699f0e0348991ee36978f7070264</anchor>
      <arglist>(Archive &amp;ar, const BT2::BitVector &amp;bv, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>load</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>2c8520eb09f488e8d87401bdec9b778e</anchor>
      <arglist>(Archive &amp;ar, BT2::BitVector &amp;bv, const unsigned int version)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialize</name>
      <anchorfile>namespaceboost_1_1serialization.html</anchorfile>
      <anchor>4d6f3b604885b5ee3fecbdeb526eeac4</anchor>
      <arglist>(Archive &amp;ar, BT2::BitVector &amp;t, const unsigned int file_version)</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>BT2</name>
    <filename>namespaceBT2.html</filename>
    <class kind="class">BT2::FileHandler</class>
    <class kind="struct">BT2::MetaData</class>
    <class kind="class">BT2::BitVector</class>
    <class kind="class">BT2::FileObject</class>
    <class kind="class">BT2::Chunk</class>
    <class kind="class">BT2::MetaFile</class>
    <class kind="class">BT2::PartialAlgorithm</class>
    <member kind="function">
      <type>BitVector</type>
      <name>bitvector_from_string</name>
      <anchorfile>namespaceBT2.html</anchorfile>
      <anchor>ec25942d9d92338be4fb202ca194a966</anchor>
      <arglist>(const std::string &amp;data, size_t bitlen=0)</arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>bitvector_to_string</name>
      <anchorfile>namespaceBT2.html</anchorfile>
      <anchor>9fd2fcf7e644c2139520255384484efe</anchor>
      <arglist>(const BitVector &amp;bits)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BT2::FileHandler</name>
    <filename>classBT2_1_1FileHandler.html</filename>
    <member kind="function">
      <type></type>
      <name>FileHandler</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>dfec7869adf832779520170672879fa7</anchor>
      <arglist>(const std::string &amp;diskFileName)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>FileHandler</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>0eeaef80a0045fe113f6dd773dfd6305</anchor>
      <arglist>(const MetaData &amp;metadata, FileObject::FileState state=FileObject::UNKNOWN)</arglist>
    </member>
    <member kind="function">
      <type>const MetaData &amp;</type>
      <name>getMetaData</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>a4a07eeba504bf755869b2a8019d314a</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>MetaData &amp;</type>
      <name>getMetaData</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>f745554a4d01c950a0ab2b64ce3b3151</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>getFilename</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>29210045cc9b5662f594428456434152</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>FileObject &amp;</type>
      <name>getFileObject</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>020e8c893abc916e887be50e5b554215</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>PartialAlgorithm &amp;</type>
      <name>getAlgorithm</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>babaad8e64d86e9e9591e3ee30f7086d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Chunk</type>
      <name>getChunk</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>121d60f8a6260f0cb75d1db9286b1a54</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>putChunk</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>7792d8c126f0b418863ff36ac787c3c9</anchor>
      <arglist>(const Chunk &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isEndGame</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>7d412144e0c2cde9e17572424486edc4</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isDone</name>
      <anchorfile>classBT2_1_1FileHandler.html</anchorfile>
      <anchor>ea49a7edd3a147d4696a5c5074283216</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BT2::MetaData</name>
    <filename>structBT2_1_1MetaData.html</filename>
    <class kind="struct">BT2::MetaData::FileData</class>
    <member kind="typedef">
      <type>std::vector&lt; BitVector &gt;</type>
      <name>ChunkFunctionals</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>269588e235cd3905853e49aa4bf256f9</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetaData</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>2b2901553395723345c6c2f5681bb6c8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetaData</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>d2d7c64b558701555fa5ccea3855608d</anchor>
      <arglist>(const std::string &amp;fileName, const std::string &amp;diskFileName, size_t chunkSize, size_t fileSize=0, const std::vector&lt; BitVector &gt; &amp;funcs=std::vector&lt; BitVector &gt;())</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetaData</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>8b39af4a7b2b042b5f10fc0c0bda6e64</anchor>
      <arglist>(const std::string &amp;fileName, const std::string &amp;diskFileName, size_t chunkSize, const std::vector&lt; FileData &gt; &amp;containedFiles, const std::vector&lt; BitVector &gt; &amp;funcs=std::vector&lt; BitVector &gt;())</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetaData</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>e7bd7882d02fcae6ef64c7646b391530</anchor>
      <arglist>(const MetaData &amp;other)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isSingleFile</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>05b7c25499a65d123185cb6e772b0d8d</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getContainedFilePath</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>641f6839cee2da1ecc5906a359f2648c</anchor>
      <arglist>(int index) const </arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; BitVector &gt;</type>
      <name>getFunctionals</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>d950fec7d7f13d1de4292cfccf0b99ea</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>getMaxChunkCount</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>d50068b8294a58c86e250454b99b6ecc</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>size_t</type>
      <name>getChunkCount</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>8835f702f7544547501c8fde4dcff9af</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>const BitVector</type>
      <name>getCompletePieces</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>1e38b2a5010e17ec9c1b3dbce27a2193</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isComplete</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>8ce5ff1f988eac00fdc4c8a6856f49a9</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dump</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>3a4c471c882794a2a047ffa6446ae499</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>fileName</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>17298c60264f1c81b1d40415b0dd727e</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>diskFileName</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>d7cc021a9dd4fc81e294085797010fa0</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>chunkSize</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>ff28266152f7bd835778f3605de5c62d</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; FileData &gt;</type>
      <name>containedFiles</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>6d3d2246bb1219c55676555cbd4e9549</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>fileSize</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>65685e8789b6b6a7671aab1c6ee1cccf</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>std::vector&lt; BitVector &gt;</type>
      <name>chunkFunctionals</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>022d3f5fb547f6f90f2068471d2f584f</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static const BitVector</type>
      <name>EMPTY_FUNCTIONAL</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>11fec422b350c95c9abfd3e0f6ba9852</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend class</type>
      <name>boost::serialization::access</name>
      <anchorfile>structBT2_1_1MetaData.html</anchorfile>
      <anchor>c98d07dd8f7b70e16ccb9a01abf56b9c</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="struct">
    <name>BT2::MetaData::FileData</name>
    <filename>structBT2_1_1MetaData_1_1FileData.html</filename>
    <member kind="function">
      <type></type>
      <name>FileData</name>
      <anchorfile>structBT2_1_1MetaData_1_1FileData.html</anchorfile>
      <anchor>3cae53935ba141c9a885578c47fe16fd</anchor>
      <arglist>(const std::string &amp;_path=&quot;&quot;, size_t _offset=0, size_t _size=0, size_t _index=0)</arglist>
    </member>
    <member kind="variable">
      <type>std::string</type>
      <name>path</name>
      <anchorfile>structBT2_1_1MetaData_1_1FileData.html</anchorfile>
      <anchor>86c1de01b8f4ed2feaae21f70a2a5ef2</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>offset</name>
      <anchorfile>structBT2_1_1MetaData_1_1FileData.html</anchorfile>
      <anchor>58e1be5ad7a32b561d886d9872f95e3c</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>size</name>
      <anchorfile>structBT2_1_1MetaData_1_1FileData.html</anchorfile>
      <anchor>cec372d6d0fd4102f634ba2a26b470f1</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable">
      <type>size_t</type>
      <name>index</name>
      <anchorfile>structBT2_1_1MetaData_1_1FileData.html</anchorfile>
      <anchor>9f4d8e56e814479a8bc9459da04a5348</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BT2::BitVector</name>
    <filename>classBT2_1_1BitVector.html</filename>
  </compound>
  <compound kind="class">
    <name>BT2::FileObject</name>
    <filename>classBT2_1_1FileObject.html</filename>
    <member kind="enumeration">
      <name>FileState</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>2bfced05d4197a6cf6bdc2d68953da32</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>UNKNOWN</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>2bfced05d4197a6cf6bdc2d68953da3220a65e81e34fbfd3f7db36e4dde6b2f5</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>NEW</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>2bfced05d4197a6cf6bdc2d68953da32c7e001ad695242f973ce980fa67c17ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>INCOMPLETE</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>2bfced05d4197a6cf6bdc2d68953da32d9e13bd211e5162c890b6ca3cae72d2f</anchor>
      <arglist></arglist>
    </member>
    <member kind="enumvalue">
      <name>COMPLETE</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>2bfced05d4197a6cf6bdc2d68953da32531d09589e5a75c15dce61a6cdb643db</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>FileObject</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>2717d4a64d866a467e8563108df8602b</anchor>
      <arglist>(const std::string &amp;filename)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>FileObject</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>eafd805571a63797c6cd35e66e46b4e3</anchor>
      <arglist>(const MetaData &amp;metadata, FileState state=UNKNOWN)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~FileObject</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>973f2da7733b30f626233c9be35b4a88</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const MetaData &amp;</type>
      <name>getMetaData</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>c5c3bc22233f966b3988043c27ce1d1b</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>MetaData &amp;</type>
      <name>getMetaData</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>388c759ce8f4e0957de2f5f17a5deb59</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>std::vector&lt; BitVector &gt;</type>
      <name>getFunctionals</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>e18391430e136f2248d121dbe07fdd23</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>const std::vector&lt; BitVector &gt; &amp;</type>
      <name>getFunctionalMatrix</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>853382155fdf0e26b7843d089123b169</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>Chunk</type>
      <name>getChunk</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>10e5a015eaf7742be3dbb1e5731dcfa7</anchor>
      <arglist>(int chunk_num)</arglist>
    </member>
    <member kind="function">
      <type>Chunk</type>
      <name>getChunk</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>42c3fd37b4c4f4ccf2a23401b1c3b3ad</anchor>
      <arglist>(const std::vector&lt; BitVector &gt; &amp;functionals)</arglist>
    </member>
    <member kind="function">
      <type>Chunk</type>
      <name>getChunk</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>41850ad8931b6ecda22d349a7ab59e66</anchor>
      <arglist>(const BitVector &amp;a, const BitVector &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>putChunk</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>b3609144ee8cd6cb0308575e0650f2e7</anchor>
      <arglist>(const Chunk &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>putCombinedChunks</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>58f4a42c08bbcdd027e062bc460f88c1</anchor>
      <arglist>(const std::vector&lt; BitVector &gt; &amp;functionals)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>putCombinedChunks</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>2faf855ac6a44320f198f4e3847d1c0d</anchor>
      <arglist>(const std::vector&lt; BitVector &gt; &amp;functionals, const Chunk &amp;chunk)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>completeFiles</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>296e1832c804b0c8f30ab6766afa4d50</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>closeFiles</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>1d4c3c278b86880cfe7b6881644d0b17</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reopenFiles</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>e6c329d739686ce622266bb2e126b722</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>persist</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>13dd99921e3bea2452f1c56b7ede0dc5</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>size_t</type>
      <name>getFileSize</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>a126e142deecb872ee0d0a7b3d0ecd68</anchor>
      <arglist>(size_t index)</arglist>
    </member>
    <member kind="function" protection="protected">
      <type>size_t</type>
      <name>getFilesSize</name>
      <anchorfile>classBT2_1_1FileObject.html</anchorfile>
      <anchor>3f5b290ae32fb8f5af3ab4a7e3076aff</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BT2::Chunk</name>
    <filename>classBT2_1_1Chunk.html</filename>
    <member kind="function">
      <type></type>
      <name>Chunk</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>e00a5524d9aa11fd1bffad6d8f9ca17c</anchor>
      <arglist>(size_t index, size_t maxChunks, const std::string &amp;data)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Chunk</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>c3d597acf6a86c457851dfde9a643ecc</anchor>
      <arglist>(BitVector functional, const std::string &amp;data)</arglist>
    </member>
    <member kind="function">
      <type>Chunk</type>
      <name>operator^</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>b0e0c2c5244bff9cad811013a37a2470</anchor>
      <arglist>(const Chunk &amp;rhs) const </arglist>
    </member>
    <member kind="function">
      <type>Chunk &amp;</type>
      <name>operator^=</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>7062fab0e80bdde930f4b73738643c40</anchor>
      <arglist>(const Chunk &amp;lhs)</arglist>
    </member>
    <member kind="function">
      <type>const std::string &amp;</type>
      <name>getData</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>6345eb0976e4a9f0b89c4c92c0048b4c</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>const BitVector &amp;</type>
      <name>getFunctional</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>42d5923f8cd545bf96c107a2ef677c47</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>std::string &amp;</type>
      <name>getData</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>ae675d7e467c8e14a6fbc1fed8e40d3d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>BitVector &amp;</type>
      <name>getFunctional</name>
      <anchorfile>classBT2_1_1Chunk.html</anchorfile>
      <anchor>4a01ee3f25bfc4039155c0c6a1a73402</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BT2::MetaFile</name>
    <filename>classBT2_1_1MetaFile.html</filename>
    <member kind="function">
      <type></type>
      <name>MetaFile</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>7e2e8115681646a0ec82718c09a50d73</anchor>
      <arglist>(const std::string &amp;baseFileName)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetaFile</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>47e879177b4168b32468d3f457662a20</anchor>
      <arglist>(const std::string &amp;baseFileName, const std::string &amp;diskFileName)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>MetaFile</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>ad79cf08bae8109c317b5f9aa347150f</anchor>
      <arglist>(MetaData *metadata)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~MetaFile</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>8ce6affe7b3af94502604cbc972a0f9d</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isInitialized</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>0d9276f070ec8dba3e13055ca9b8e349</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>MetaData &amp;</type>
      <name>getMetaData</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>29080fed37b06245b76cf0722006c24a</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>const MetaData &amp;</type>
      <name>getMetaData</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>b5ae6d57ed0aebdde9464442e11560e6</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>std::string</type>
      <name>getFileName</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>4c040d0af7a473103ff92a146a91d9b6</anchor>
      <arglist>() const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>persist</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>4b8a0921a5331fdfd3e52f2f4991d4d3</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static bool</type>
      <name>checkMetaFileExists</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>6a8ce76c08cfe583522af715d3d21d32</anchor>
      <arglist>(const std::string &amp;baseFileName)</arglist>
    </member>
    <member kind="function" static="yes">
      <type>static std::string</type>
      <name>getMetaFilePathForFile</name>
      <anchorfile>classBT2_1_1MetaFile.html</anchorfile>
      <anchor>c25eea715405c3f8748abf60bcac9f6f</anchor>
      <arglist>(const std::string &amp;baseFileName)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BT2::PartialAlgorithm</name>
    <filename>classBT2_1_1PartialAlgorithm.html</filename>
    <member kind="function">
      <type></type>
      <name>PartialAlgorithm</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>dad3f99897b3346117151d638fc09c00</anchor>
      <arglist>(FileObject &amp;file)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~PartialAlgorithm</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>73f54736acb26a25a5061b9645a6d113</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>canAddFunctional</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>37c75717bab72f1c72ca74af04a68ddc</anchor>
      <arglist>(const BitVector &amp;functional)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>addChunk</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>beb276fa0fbe137e5602fe3477e86487</anchor>
      <arglist>(const Chunk &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isEndGame</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>0de3ec2e958b64391161cad43c7c2b24</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>Chunk</type>
      <name>getChunk</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>47bfd5c5e145e86085c24ac8db6069f8</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>isDone</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>474ca85385572d6505064798e5d91aa9</anchor>
      <arglist>()</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>wrapUp</name>
      <anchorfile>classBT2_1_1PartialAlgorithm.html</anchorfile>
      <anchor>43f2022fca25eca19e75aa7ba0acd4fb</anchor>
      <arglist>()</arglist>
    </member>
  </compound>
</tagfile>
