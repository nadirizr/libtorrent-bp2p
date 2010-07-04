#ifndef BITTORRENT2_COMMON_METADATA_H
#define BITTORRENT2_COMMON_METADATA_H

#include <vector>
#include <string>
#include <common/BitVector.h>

#include <boost/serialization/base_object.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
// versioning does not work for some reason
//#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

namespace BT2 {

/**
 * Meta-data about a file
 * Contains data such as: file size, chunk size
 * chunk lists, etc.
 */
struct MetaData {

public:
    typedef std::vector<BitVector> ChunkFunctionals;

    /**
     * Represents an empty functional - used as a place holder
     */
    const static BitVector EMPTY_FUNCTIONAL;

public:
    /**************** Fields *******************/

    /**
     * File name that we belong to
     */
    std::string fileName;

    /**
     * File name on disk
     */
    std::string diskFileName;

    /**
     * Logical chunk size
     */
    size_t chunkSize;

    /**
     * Contains the data for a single file contained in this torrent.
     */
    struct FileData {
        FileData(const std::string& _path = "", size_t _offset = 0, size_t _size = 0, size_t _index = 0)
            : path(_path), offset(_offset), size(_size), index(_index) { }

        std::string path; // the relative path of the contained file
        size_t offset;    // the offset within the torrent where the file starts
        size_t size;      // the size of the file
        size_t index;     // the index of the file in the torrent
    };

    /**
     * An array of the details of every file contained within this torrent.
     */
    std::vector<FileData> containedFiles;

    /**
     * Total target file size
     */
    size_t fileSize;

    /**
     * All chunk functionals (including place holders).
     * This is the functional matrix, containing empty functionals that are not
     * yet used.
     */
    std::vector<BitVector> chunkFunctionals;


    /************************ Methods **********************/

    /**
     * Default Constructor.
     */
    MetaData();
    /**
     * Trivial Constructor from the data itself, for single file torrents.
     * Optional parameters are fileSize and the functional matrix (which can
     * both be calculated in some cases by other classes).
     */
    MetaData(const std::string& fileName, const std::string& diskFileName,
            size_t chunkSize, size_t fileSize = 0,
            const std::vector<BitVector>& funcs = std::vector<BitVector>());
    /**
     * Trivial Constructor from the data itself, for multi file torrents.
     * Optional parameters are the functional matrix (which can be calculated
     * in some cases by other classes).
     */
    MetaData(const std::string& fileName, const std::string& diskFileName,
            size_t chunkSize, const std::vector<FileData>& containedFiles,
            const std::vector<BitVector>& funcs = std::vector<BitVector>());
    /**
     * Copy Constructor.
     */
    MetaData(const MetaData &other);

    /**
     * Returns true if the torrent contains a single file.
     */
    bool isSingleFile() const;

    /**
     * Returns the full disk path for the given index of a contained file.
     *
     * @param index the index of the file
     * @param the full path on disk
     */
    std::string getContainedFilePath(int index) const;

    /**
     * Get the vector of actual chunks that exist.
     */
    std::vector<BitVector> getFunctionals() const;

    /**
     * Get the max number of chunks.
     */
    size_t getMaxChunkCount() const;

    /**
     * Get the actual amount of chunks we have
     */
    size_t getChunkCount() const;

    /**
     * Returns a vector representing completed chunk-map of the file
     * A completed chunk's functional has only one bit set
     */
    const BitVector getCompletePieces() const;

    /**
     * Returns true if the file is complete, and false otherwise.
     */
    bool isComplete() const;

    /**
     * Prints the functionals to stderr for debugging.
     */
    void dump() const;

private:
    /**
     * Magic that identifies metadata
     */
    const static std::string METADATA_MAGIC;

private:
    // serialization
    friend class boost::serialization::access;

    template <class Archive>
    void save(Archive &ar, const unsigned int version) const {

        ar & MetaData::METADATA_MAGIC;
        ar & version;
        ar & fileName;
        ar & diskFileName;
        ar & containedFiles;
        ar & fileSize;
        ar & chunkSize;
        ar & chunkFunctionals;

    }
    template <class Archive>
    void load(Archive &ar, const unsigned int version) {

        std::string loadMAGIC;
        unsigned int loadVersion;

        ar & loadMAGIC;
        assert(loadMAGIC == MetaData::METADATA_MAGIC);

        ar & loadVersion;
        assert(loadVersion == version);

        ar & fileName;
        ar & diskFileName;
        ar & containedFiles;
        ar & fileSize;
        ar & chunkSize;
        ar & chunkFunctionals;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()

}; /* struct MetaData */

} /* namespace BT2 */

/*
 * These are serialization methods for MetaData::FileData struct.
 */
namespace boost {
namespace serialization {

template<class Archive>
void save(Archive &ar, const BT2::MetaData::FileData &fd, const unsigned int version) {
    ar & fd.path;
    ar & fd.offset;
    ar & fd.size;
    ar & fd.index;
}

template<class Archive>
void load(Archive &ar, BT2::MetaData::FileData &fd, const unsigned int version) {
    ar & fd.path;
    ar & fd.offset;
    ar & fd.size;
    ar & fd.index;
}

}
}
BOOST_SERIALIZATION_SPLIT_FREE(BT2::MetaData::FileData)

#ifndef __doxygen
BOOST_CLASS_TRACKING(BT2::MetaData, boost::serialization::track_never)
#endif


#endif /* BITTORRENT2_COMMON_METADATA_H */

