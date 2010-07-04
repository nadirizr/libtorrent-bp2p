#include "FileObject.h"
#include <common/Logger.h>
#include <boost/filesystem.hpp>

#include <stdlib.h>
#include <stdexcept>
#include <cstring>
using namespace std;

#ifdef WIN32
# include <io.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <share.h>
  // TODO(mishas): maybe implement ftruncate?
  /** Implementing BSD/Linux system call for windows */
  int truncate(const char* path, off_t length) {
     int fh;
     if (_sopen_s(&fh, path, _O_RDWR, _SH_DENYNO, _S_IREAD | _S_IWRITE ) == 0 ) {
        if (_chsize(fh, length) != 0) {
           return -1;
        }
        _close(fh);
     }
     return 0;
  }
#else
# include <unistd.h>
#endif


namespace BT2 {


FileObject::FileObject(const std::string& filename) :
      m_metafile(new MetaFile(filename)),
      m_metadata(&m_metafile->getMetaData()) {
    initializeFiles();
    initializeMetaData();
}

FileObject::FileObject(const MetaData& metadata, FileObject::FileState state)
	: m_metafile(NULL), m_metadata(new MetaData(metadata)) {
    // first, initialize the actual files according to the meta data
    initializeFiles();
    // attempt to initialize a metafile
    initializeMetaData();

    // if the state is unknown, attempt to determine what state we are in
    if (state == UNKNOWN) {
        if (getFilesSize() == 0 && m_metadata->fileSize != 0) {
            state = NEW;
        }
        else if (m_metadata->isComplete() && m_metadata->fileSize == getFilesSize()) {
            state = COMPLETE;
        }
        else if (m_metadata->getChunkCount() > 0) {
            state = INCOMPLETE;
        }
        else {
            throw std::runtime_error("FileObject::FileObject: could not determine file state");
        }
    }

    // initialize the file data and meta-data according to the file state
    if (state == NEW) {
        cerr << "// creating new file '" << m_metadata->fileName << "'" << endl;
        createEmptyMatrix();
    }
    else if (state == INCOMPLETE) {
        cerr << "// creating incomplete file '" << m_metadata->fileName << "'" << endl;
        cerr << "// m_metafile = " << (int)m_metafile << endl;
    }
    else if (state == COMPLETE) {
        cerr << "// creating complete file '" << m_metadata->fileName << "'" << endl;
        cerr << "// m_metafile = " << (int)m_metafile << endl;
        m_metadata->fileSize = getFilesSize();
        for (size_t i = 0; i < m_metadata->containedFiles.size(); ++i) {
            m_metadata->containedFiles[i].size = getFileSize(i);
        }
        createIdentityMatrix();
    }
    else {
        throw std::runtime_error("FileObject::FileObject: unknown file state given");
    }

    // save the meta-file
    persist();
}


FileObject::~FileObject() {
    closeFiles();
    for (size_t i = 0; i < m_files.size(); ++i) {
        if (m_files[i]) {
            delete m_files[i];
            m_files[i] = NULL;
        }
    }

    if (m_metafile != NULL) {
        delete m_metafile;
    }
    else if (m_metadata != NULL) {
        delete m_metadata;
    }
}

std::vector<BitVector> FileObject::getFunctionals() const {
    return getMetaData().getFunctionals();
}

const std::vector<BitVector>& FileObject::getFunctionalMatrix() const {
    return getMetaData().chunkFunctionals;
}

Chunk FileObject::getChunk(int chunk_num) {
    return readChunkFromFiles(chunk_num);
}

Chunk FileObject::getChunk(const std::vector<BitVector>& functionals) {
    // if there are no functionals - error
    if (functionals.size() < 1) {
        throw std::runtime_error("FileObject::getChunk: empty functional list given as argument");
    }

    Logger::getLog("FileObject").debugStream()
        << "FileObject::getChunk(" << functionals.size() << " functionals)";

    // read the first real chunk
    int ci = calculateFunctionalIndex(functionals[0]);
    Chunk c = readChunkFromFiles(ci);
    for (size_t i = 1; i < functionals.size(); ++i) {
        // retrieve the current real chunk and combine into the first
        int ci = calculateFunctionalIndex(functionals[i]);
        c ^= readChunkFromFiles(ci);
    }

    return c;
}

Chunk FileObject::getChunk(const BitVector& a, const BitVector& b) {
    // read the first chunk
    Chunk c = readChunkFromFiles(calculateFunctionalIndex(a));
    c ^= readChunkFromFiles(calculateFunctionalIndex(b));
    return c;
}

void FileObject::putChunk(const Chunk& c) {
    //Logger::getLog("FileObject").debugStream() << "FileObject::putChunk():";
    //Logger::getLog("FileObject").debugStream()
    //    << "--- c.getFunctional() = " << c.getFunctional() << " (" << c.getFunctional().size() << ")";

    // write the chunk at the correct place
    size_t index = calculateFunctionalIndex(c.getFunctional());
    assert(funcs()[index] == MetaData::EMPTY_FUNCTIONAL);
    writeChunkToFiles(c, index);

    // persist at the end
    persist();
}

void FileObject::putCombinedChunks(const std::vector<BitVector>& functionals) {
    assert(functionals.size() > 0);

    Chunk c = readChunkFromFiles(calculateFunctionalIndex(functionals[0]));
    for (size_t i = 1; i < functionals.size(); ++i) {
    	c ^= readChunkFromFiles(calculateFunctionalIndex(functionals[i]));
    }

    // replace the chunks
    replaceChunk(c);
}

void FileObject::putCombinedChunks(
        const std::vector<BitVector>& functionals, const Chunk& chunk) {
    // combine the chunks
    Chunk c (chunk);
    for (size_t i = 0; i < functionals.size(); ++i) {
        c ^= readChunkFromFiles(calculateFunctionalIndex(functionals[i]));
    }

    // insert the chunk
    putChunk(c);
}

void FileObject::completeFiles() {
    // close the files first
    closeFiles();

    // if we have one file, truncate it
    if (m_metadata->isSingleFile()) {
        truncate(getMetaData().diskFileName.c_str(), getMetaData().fileSize);
    }
    else { // if we have multiple files, truncate each one
        std::vector<MetaData::FileData>& files = m_metadata->containedFiles;
        for (size_t i = 0; i < files.size(); ++i) {
            truncate(m_metadata->getContainedFilePath(i).c_str(), files[i].size);
        }
    }

    // reopen the files
    reopenFiles();

    // dispose of the metafile
    string metafilepath = m_metafile->getFileName();
    m_metadata = new MetaData(*m_metadata);
    delete m_metafile;
    m_metafile = NULL;
    unlink(metafilepath.c_str());
}

void FileObject::closeFiles() {
    for (size_t i = 0; i < m_files.size(); ++i) {
        if (m_files[i]->is_open()) {
            m_files[i]->close();
        }
    }

    persist();
}

void FileObject::reopenFiles() {
    if (m_metadata->isSingleFile()) {
        if (!m_files[0]) {
            m_files[0] = new boost::filesystem::fstream();
        }

        cerr << "// FileObject::reopenFiles: single-file torrent '" << m_metadata->diskFileName << "'" << endl;
        if (!m_files[0]->is_open()) {
            m_files[0]->open(
                    m_metadata->diskFileName, ios::in | ios::out | ios::binary);
            assert(m_files[0]->is_open() && !m_files[0]->bad());
        }
    }
    else {
        for (size_t i = 0; i < m_files.size(); ++i) {
            if (!m_files[i]) {
                m_files[i] = new boost::filesystem::fstream();
            }
            if (!m_files[i]->is_open()) {
                m_files[i]->open(
                        m_metadata->getContainedFilePath(i),
                        ios::in | ios::out | ios::binary);
                assert(m_files[i]->is_open() && !m_files[i]->bad());
            }
        }
    }
}

void FileObject::initializeFiles() {
    const string &fileName = getMetaData().diskFileName;

    // destory all current handles
    closeFiles();
    for (size_t i = 0; i < m_files.size(); ++i) {
        delete m_files[i];
    }

    // if we have contained files, treat diskFileName as a directory to them
    if (!m_metadata->isSingleFile()) {
        m_files.resize(m_metadata->containedFiles.size());

        // create the directory
        if (!boost::filesystem::exists(fileName)) {
            Logger::getLog("FileObject").infoStream() << "Creating new directory: " << fileName;
            boost::filesystem::create_directories(fileName);
            assert(boost::filesystem::exists(fileName));
        }

        // create the files
        for (size_t i = 0; i < m_metadata->containedFiles.size(); ++i) {
            std::string filepath = m_metadata->getContainedFilePath(i);
            if (!boost::filesystem::exists(filepath)) {
                // first create all directories up to the file
                std::string parent_path = boost::filesystem::path(fileName).parent_path().string();
                boost::filesystem::create_directories(parent_path);
                assert(boost::filesystem::exists(parent_path));

                // then the file
                Logger::getLog("FileObject").infoStream() << "Creating new file: " << filepath;
                boost::filesystem::ofstream new_file(filepath, ios::out);
                assert(new_file.is_open());
                new_file.close();
            }
        }
    }
    else { // no contained files, only the one pointed to by diskFileName
        m_files.resize(1);

        if (!boost::filesystem::exists(fileName)) {
            // first create all directories up to the file
            std::string parent_path = boost::filesystem::path(fileName).parent_path().string();
            boost::filesystem::create_directories(parent_path);
            assert(boost::filesystem::exists(parent_path));
            
            // then the file
            Logger::getLog("FileObject").infoStream() << "Creating new file: " << fileName;
            boost::filesystem::ofstream new_file(fileName, ios::out);
            assert(new_file.is_open());
            new_file.close();
        }
    }

    // reopen the file
    reopenFiles();
}

void FileObject::initializeMetaData() {
    // handle the metafile
    // if there is not metafile, create one
    // if there is a metafile, load it and override the metadata
    if (m_metafile == NULL) {
        assert(m_metadata != NULL);

        // new file - create the metafile from the metadata
        if (m_metadata->getChunkCount() == 0 && getFilesSize() == 0) {
            m_metafile = new MetaFile(m_metadata);
        }
        // existing file initialized from meta-data - override meta-data with
        // file data if it's the same one
        else if (MetaFile::checkMetaFileExists(m_metadata->fileName)) {
            m_metafile = new MetaFile(m_metadata->fileName);

            // check if the data corresponds to the same file
            if ((m_metafile->getMetaData().chunkSize == m_metadata->chunkSize) &&
                (m_metafile->getMetaData().fileSize  == m_metadata->fileSize ) &&
                (m_metafile->getMetaData().fileName  == m_metadata->fileName )) {
                m_metadata = &m_metafile->getMetaData();
            }
            else {
                m_metafile = new MetaFile(m_metadata);
            }
        }
        // incomplete file with no metafile - irrecoverable error as of now
        // TODO think of this
        else if (m_metafile == NULL && !m_metadata->isComplete()) {
            throw std::runtime_error("FileObject::initialize: missing metafile and incomplete file");
        }
    }

    // save the meta-file
    persist();
}

void FileObject::persist() {
    if (m_metafile != NULL) {
        m_metafile->persist();
    }
}

void FileObject::replaceChunk(const Chunk& c) {
	Logger::getLog("FileObject").debugStream() << "Replacing some chunk with " << c.getFunctional();
    // write the chunk at the correct place
	size_t index = calculateFunctionalIndex(c.getFunctional());
    writeChunkToFiles(c, index);
}

Chunk FileObject::readChunkFromFiles(size_t index) {
    assert(index >= 0 && index < getMetaData().getMaxChunkCount());
    
    size_t chunkSize = getMetaData().chunkSize;
    
    // if the index refers to a chunk we do not yet have, return an empty chunk
    if (funcs()[index] == MetaData::EMPTY_FUNCTIONAL) {
        return Chunk(
                BitVector(getMetaData().getMaxChunkCount()),
                string(chunkSize, '\0'));
    }

    // make sure files are open
    reopenFiles();

    // find out what files we need to read
    std::vector<MetaData::FileData> files = calculateFilesForIndex(index);
    cerr << "// FileObject::readChunkFromFiles: files = " << endl;
    for (size_t i = 0; i < files.size(); ++i) {
        cerr << "// --> files[" << i << "].index = " << files[i].index << endl;
        cerr << "// --> files[" << i << "].offset = " << files[i].offset << endl;
        cerr << "// --> files[" << i << "].size = " << files[i].size << endl;
    }

    // read each portion of each file into our buffer
    char *buf = new char[chunkSize];
    char *buf_ptr = buf;
    for (size_t i = 0; i < files.size(); ++i) {
        m_files[files[i].index]->seekg(files[i].offset);
        m_files[files[i].index]->read(buf_ptr, files[i].size);
        size_t read_bytes = m_files[files[i].index]->gcount();
        if (read_bytes < files[i].size) {
            memset(buf_ptr + read_bytes, '\0', files[i].size - read_bytes);
            // We read beyond the eof, we need to clear flags!
            m_files[files[i].index]->clear();
        }
        buf_ptr += files[i].size;
    }

    // simply place the buffer now into a chunk
    Chunk c(funcs()[index], string(buf, chunkSize));
    delete[] buf;

    return c;
}

void FileObject::writeChunkToFiles(const Chunk& c, size_t index) {
    assert(index >= 0 && index < getMetaData().getMaxChunkCount());

    // make sure file is open
    reopenFiles();

    const BitVector &func = c.getFunctional();
    size_t chunkSize = getMetaData().chunkSize;
    assert(chunkSize == c.getData().size());

    // put functional in place
    funcs()[index] = func;

    // find out what files we need to write to
    std::vector<MetaData::FileData> files = calculateFilesForIndex(index);

    // write each portion of the new chunk into our files
    const char *buf_ptr = c.getData().c_str();
    for (size_t i = 0; i < files.size(); ++i) {
        m_files[files[i].index]->seekp(files[i].offset);
        m_files[files[i].index]->write(buf_ptr, files[i].size);
        m_files[files[i].index]->flush();
        assert(!m_files[files[i].index]->bad());
    }
}

size_t FileObject::getFileSize(size_t index) {
    if (index < 0 || index >= m_files.size()) {
        return 0;
    }

    reopenFiles();

    std::ifstream::pos_type begin, end;

    m_files[index]->seekg(0, std::ios::beg);
    begin = m_files[index]->tellg();
    m_files[index]->seekg(0, std::ios::end);
    end = m_files[index]->tellg();

    return static_cast<size_t>(end - begin);
}

size_t FileObject::getFilesSize() {
    size_t size = 0;
    for (size_t i = 0; i < m_files.size(); ++i) {
        cerr << "// FileObject::getFileSize: getFileSize(" << i << ") = " << getFileSize(i) << endl;
        size += getFileSize(i);
    }
    return size;
}

void FileObject::createIdentityMatrix()
{
	std::vector<BitVector>& matrix = m_metadata->chunkFunctionals;

    // delete existing matrix
    matrix.resize(0);

    // create identity matrix
	size_t n = (m_metadata->fileSize + m_metadata->chunkSize - 1) / m_metadata->chunkSize;
	matrix.resize(n);
	for (size_t i = 0; i < matrix.size(); i++) {
		matrix[i].resize(n);
		matrix[i][i] = 1;
	}
}

void FileObject::createEmptyMatrix()
{
	std::vector<BitVector>& matrix = m_metadata->chunkFunctionals;

    // delete existing matrix
    matrix.resize(0);

    // create an empty one
	size_t n = (m_metadata->fileSize + m_metadata->chunkSize - 1) / m_metadata->chunkSize;
	matrix.resize(n);
}

size_t FileObject::calculateFunctionalIndex(const BitVector& functional) {
    size_t index = functional.find_first();
    assert(index != BitVector::npos);
    return index;
}

std::vector<MetaData::FileData> FileObject::calculateFilesForIndex(size_t index) {
    std::vector<MetaData::FileData> files;
    size_t begin = index * m_metadata->chunkSize;
    size_t end = (index + 1) * m_metadata->chunkSize;

    if (m_metadata->isSingleFile()) {
        // if this is only one file, then this is easy
        files.push_back(MetaData::FileData(
                    m_metadata->diskFileName, begin, m_metadata->chunkSize));
    }
    else {
        // if there are multiple files, we need to find the files that are
        // affected, and adjust their offsets and sizes
        std::vector<MetaData::FileData>& all_files = m_metadata->containedFiles;

        // find the first file
        size_t i = 0;
        for (i = 0; (i < all_files.size()) &&
                    ((all_files[i].offset + all_files[i].size) < begin); ++i);

        // gather all the files that are in range
        for (; (i < all_files.size()) && (all_files[i].offset < end); ++i) {
            files.push_back(all_files[i]);
            files[files.size() - 1].offset = 0;
        }

        // fix the offset and size of the first file
        files[0].offset = begin - all_files[files[0].index].offset;
        files[0].size -= files[0].offset;
        // fix the size of the last file
        files[files.size() - 1].size = 
            end - (all_files[files[files.size() - 1].index].offset +
                   files[files.size() - 1].offset);
    }

    return files;
}

std::vector<BitVector>& FileObject::funcs()
{
  return m_metadata->chunkFunctionals;
}

} // namespace BT2

