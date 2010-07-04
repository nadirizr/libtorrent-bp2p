#ifndef BITTORRENT2_FILESYSTEM_METAFILE_H
#define BITTORRENT2_FILESYSTEM_METAFILE_H


#include <common/MetaData.h>

// for serialization
#include <boost/filesystem/fstream.hpp>


namespace BT2 {

/**
 * The MetaFile contains meta-data about the actual file
 * The metadata includes target file size, chunk size,
 * chunk lists, and anything else we wish to make available
 *
 * For now, it's just the above three
 */
class MetaFile {

public:

    /**
     * Creates a MetaFile.
     *
     * @param baseFileName name of the file that the metafile belongs to
     */
    MetaFile(const std::string& baseFileName);
    /**
     * Creates a MetaFile
     * 
     * @param baseFileName real file name of the file (if baseFileName is not equal)
     * @param diskFileName name of the file on disk that the metafile belongs to
     */
    MetaFile(const std::string& baseFileName, const std::string& diskFileName);
    /**
     * Creates a MetaFile from the given MetaData, creating the file as needed.
     */
    MetaFile(MetaData* metadata);
    /**
     * Dtor, saves self to file
     */
    ~MetaFile();

    /**
     * Returns true if the metadata was loaded from a file
     * or otherwise initialized
     */
    bool isInitialized() const;

    /**
     * Returns the metadata
     */
    MetaData& getMetaData();
    /**
     * Returns the metadata, const version
     */
    const MetaData& getMetaData() const;

    /**
     * Returns the metafile path.
     */
    std::string getFileName() const;

    /**
     * Saves all data to file (and makes a copy of old file, for percistancy)
     */
    void persist();

    /**
     * Static function checks if a meta-file exists for the given file name.
     *
     * @param baseFileName target filename to check if a meta-file exists for
     * @return true if a meta-file exists
     */
    static bool checkMetaFileExists(const std::string& baseFileName);

    /**
     * Static function returns the full path of the meta file corresponding to
     * the given file name.
     *
     * @param diskFileName target filename to get the meta-file for
     * @return the meta-file path corresponding to the given file name
     */
    static std::string getMetaFilePathForFile(const std::string& baseFileName);

private:
    /**
     * Initializes the metafile
     */
    void initialize(const std::string& baseFileName, const std::string& diskFileName);
    /**
     * Loads the metadata from file
     */
    void performLoad();
    /**
     * Saves the metadata to file
     */
    void performSave();

private:

    MetaData* m_metadata;
    bool m_initialized;

    /**
     * Metafile stream
     */
    mutable boost::filesystem::fstream m_metafile;

private:
    /**
     * Suffix appended to metafile names
     */
    const static std::string METAFILE_SUFFIX;

    /**
     * The path in which to put our metafiles.
     */
    const static std::string METAFILE_DIRECTORY;
};

// versioning does not work for some reason
//BOOST_CLASS_VERSION(MetaFile, 1)

} /* namespace BT2 */

#endif /* BITTORRENT2_FILESYSTEM_METAFILE_H */
