#include "MetaFile.h"
#include <common/Logger.h>

#include <boost/filesystem.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>


using namespace std;
namespace FS = boost::filesystem;

namespace BT2 {

const std::string MetaFile::METAFILE_SUFFIX(".meta");
const std::string MetaFile::METAFILE_DIRECTORY("/var/bp2p/meta");

MetaFile::MetaFile(const std::string& baseFileName) 
  : m_metadata(new MetaData), m_initialized(false) {
    initialize(baseFileName, baseFileName);
}

MetaFile::MetaFile(const std::string& baseFileName, const std::string& diskFileName)
  : m_metadata(new MetaData), m_initialized(false) {
    initialize(baseFileName, diskFileName);
}

MetaFile::MetaFile(MetaData* metadata)
  : m_metadata(NULL), m_initialized(false) {
    initialize(metadata->fileName, metadata->diskFileName);
    m_metadata = metadata;
    persist();
}

void MetaFile::initialize(const std::string& baseFileName, const std::string& diskFileName) {
    std::string metaFileName = getMetaFilePathForFile(baseFileName);
    cerr << "// MetaFile::initialize: metaFileName = " << metaFileName << endl;
    bool performLoad = checkMetaFileExists(baseFileName);
    
    // verify that the meta-file path exists
    FS::create_directories(FS::path(metaFileName).parent_path().string());

    // load up the file
    if (!performLoad) {
        m_metafile.open(metaFileName, ios::out);
        assert(m_metafile.is_open());
        m_metafile.close();
    }

    m_metafile.open(metaFileName, ios::in | ios::out | ios::binary);
    assert(m_metafile.is_open() && !m_metafile.bad());

    Logger::getLog("MetaFile").debugStream() << "Initializing metafile: '" <<
        metaFileName << "'. Metafile " << (performLoad ? "exists." : "does not exist.");

    // if we actually have a metadata structure created (meaning we do not
    // get it from somewhere else later)
    if (m_metadata != NULL) {
      if (performLoad) {
         this->performLoad();
      } else {
         this->m_metadata->fileName = baseFileName;
         this->m_metadata->diskFileName = diskFileName;
         this->m_metadata->containedFiles = std::vector<MetaData::FileData>();
         this->m_metadata->chunkSize = 0;
         this->m_metadata->fileSize = 0;
         this->m_metadata->chunkFunctionals = MetaData::ChunkFunctionals();
      }
    }
}

std::string MetaFile::getFileName() const {
    return getMetaFilePathForFile(m_metadata->fileName);
}

void MetaFile::persist() {
    // TODO(mishas): should create backup too.
    this->performSave();
}

MetaFile::~MetaFile() {
    this->performSave();
    m_metafile.close();

    if (m_metadata != NULL)
        delete m_metadata;
}

bool MetaFile::isInitialized() const {
    return m_initialized;
}


void MetaFile::performLoad() {
    m_metafile.seekg(0, ios::beg);
    boost::archive::binary_iarchive ia(m_metafile);
    ia >> m_metadata;

    m_initialized = true;
}

void MetaFile::performSave() {
    m_metafile.seekp(0, ios::beg);
    boost::archive::binary_oarchive oa(m_metafile);
    oa << m_metadata;

    m_initialized = true;
}

MetaData &MetaFile::getMetaData() {
    return *m_metadata;
}

const MetaData &MetaFile::getMetaData() const {
    return *m_metadata;
}

bool MetaFile::checkMetaFileExists(const std::string& baseFileName) {
    return FS::exists(getMetaFilePathForFile(baseFileName));
}

std::string MetaFile::getMetaFilePathForFile(const std::string& baseFileName) {
    return (FS::path(METAFILE_DIRECTORY) / FS::path(baseFileName).filename()).string() + METAFILE_SUFFIX;
}

} /* namespace BT2 */
