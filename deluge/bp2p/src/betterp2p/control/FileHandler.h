#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H


#include "../common/MetaData.h"
#include "../filesystem/FileObject.h"
#include "../algorithm/PartialAlgorithm.h"

#include <string>


namespace BT2 {


/**
 * This class manages a single file from end to end, exposing only the simplest
 * and required interface for operating.
 * It encapsulates the file object handling, algorithm and state, and reveals
 * only the function that may interest a higher-order download management
 * system.
 */
class FileHandler {
public:
    /**
     * Creates a file handler for the given file name on disk.
     * This assumes the existence of a meta-file in order to manage.
     *
     * @param diskFileName the file name on disk to handle
     */
    FileHandler(const std::string& diskFileName);
    /**
     * Creates a file handler for the given meta-data and optional file state.
     *
     * @param metadata the meta-data for the file
     * @param state the file state for the file (DEFAULT - UNKNOWN)
     */
    FileHandler(const MetaData& metadata, FileObject::FileState state = FileObject::UNKNOWN);

    /**
     * Returns the metadata for the file we are downloading.
     */
    const MetaData& getMetaData() const;
    MetaData& getMetaData();

    /**
     * Returns the filename of the file we are downloading.
     */
    const std::string& getFilename() const;

    /**
     * Returns the internal FileObject for the file we are downloading.
     */
    FileObject& getFileObject();

    /**
     * Returns the internal Algorithm for the file we are downloading.
     */
    PartialAlgorithm& getAlgorithm();

    /**
     * Returns a chunk from the underlying file.
     *
     * @return the chunk
     */
    Chunk getChunk();

    /**
     * Inserts the given chunk into the underlying file.
     * This function automatically wraps up the file if it is done downloading,
     * but it is necessary to check for isEndgame and isDone to acquire the
     * status manually.
     * 
     * @param c the chunk to insert
     * @return true if the chunk contained usefull data
     */
    bool putChunk(const Chunk& c);

    /**
     * Returns whether or not the download is in endgame mode or not.
     */
    bool isEndGame();

    /**
     * Returns whether or not the download is done or not.
     */
    bool isDone();

private:
    FileObject m_fileobject;
    PartialAlgorithm m_algorithm;
};


} // namespace BT2


#endif
