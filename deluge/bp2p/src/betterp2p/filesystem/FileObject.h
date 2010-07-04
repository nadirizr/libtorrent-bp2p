#ifndef _FILEOBJECT_H_
#define _FILEOBJECT_H_

#include "Chunk.h"
#include "MetaFile.h"
#include <common/MetaData.h>

#include <boost/filesystem/fstream.hpp>
#include <boost/shared_ptr.hpp>

#include <vector>
#include <string>

namespace BT2 {

class FileObject {
public:
    /**
     * The state of the file we are downloading:
     * UNKNOWN - the state of the file is not known, to be determined later.
     * NEW - the file does not yet exist, and is to be created.
     * INCOMPLETE - some portion of the file already exists, but not all of it.
     * COMPLETE - all of the file exists on disk.
     */
	enum FileState { UNKNOWN, NEW, INCOMPLETE, COMPLETE };

    /**
     * Constructs the FileObject from the file name (torrent name).
     * An attempt is made to locate the metafile and load the metadata
     * information from there.
     *
     * @param filename the file name
     */
    FileObject(const std::string& filename);
    /**
     * Construct the FileObject from the given meta-data and file state.
     * If the file is new, the object is constructed using the meta-data of
     * the file to create, and the file state value of NEW.
     * If the file is still downloading and incomplete, the meta-data is given,
     * or a meta-file exists (meta-file overrides new meta-data), and the
     * file state value of INCOMPLETE.
     * If the file is already complete, with whatever meta-data exists and the
     * file state value of FULL.
     *
     * @param metadata the MetaData of the file
     * @param state FileObject::UNKNOWN to specify that the constructor
     *   should determine what is the correct state (DEFAULT)
     *   FileObject::NEW to specify a new file,
     *   FileObject::INCOMPLETE to specify a file still in progress,
     *   FileObject::COMPLETE to specify a complete file for seeding
     */
    FileObject(const MetaData& metadata, FileState state = UNKNOWN);
    /**
     * Destructor.
     */
    virtual ~FileObject();

    /**
     * Returns the meta-data of the file.
     *
     * @return the meta-data
     */
    const MetaData& getMetaData() const { return *m_metadata; }
    MetaData& getMetaData() { return *m_metadata; }

    /**
     * Retrieves the list of all the functionals that currently exist in the
     * file.
     *
     * @return the functional list
     */
    std::vector<BitVector> getFunctionals() const;
    /**
     * Retrieves the list of all the functionals, including empty placeholders.
     * This in fact is the complete functional matrix.
     *
     * @return the functional matrix
     */
    const std::vector<BitVector>& getFunctionalMatrix() const;

    /**
     * Returns the chunk at the given slot.
     *
     * @param chunk_num the chunk number
     * @return the chunk
     */
    Chunk getChunk(int chunk_num);
    /**
     * Generates a new chunk from the file using a combination of all the
     * functionals given.
     *
     * @param functionals the list of functionals to combine
     * @return the generated chunk
     */
    Chunk getChunk(const std::vector<BitVector>& functionals);
    /**
     * Generates a new chunk from the file using a combination of the 2
     * functionals given.
     * This is a common use case of the getChunk(vector).
     *
     * @param a the first functional
     * @param b the second functional
     * @return the generated chunk
     */
    Chunk getChunk(const BitVector& a, const BitVector& b);

    /**
     * Inserts the given chunk into the file, adding its functional to the list
     * of those that can be used to seed.
     *
     * @param c the chunk to insert
     */
    void putChunk(const Chunk& c);

    /**
     * Performs a combination (xor) of the given chunks according to
     * functionals, and inserts the resulting chunk.
     *
     * @param functionals the source functionals to combine
     */
    void putCombinedChunks(const std::vector<BitVector>& functionals);
    /**
     * Performs a combination (xor) of the given chunks according to
     * functionals, and inserts the resulting chunk.
     * The additional chunk given, will be combined with all the rest before inserting.
     *
     * @param functionals the source functionals to combine
     * @param chunk the additional chunk to combine
     */
    void putCombinedChunks(const std::vector<BitVector>& functionals, const Chunk& chunk);

    /**
     * Performs completion routines on the file.
     * This should be called when the algorithm determines that the download is
     * complete.
     */
    void completeFiles();

    /**
     * Closes the files and releases all handles.
     * This is usefull if other operations are to be done on the files, such as
     * renaming them.
     */
    void closeFiles();

    /**
     * Reopens the files after they have been closed.
     * The files must be already initialized before hand.
     */
    void reopenFiles();

    /**
     * Save all volatile information to disk.
     * Saves the meta-data file if we have one.
     */
    void persist();

protected:
    /**
     * Returns the file size of the given file index.
     */
    size_t getFileSize(size_t index);

    /**
     * Returns the size of all files.
     */
	size_t getFilesSize();

private:
    /**
     * Initializes the internal files.
     */
    void initializeFiles();

    /**
     * Initializes the internal metadata
     */
    void initializeMetaData();

    /**
     * Replaces an existing chunk (at the appropriate place) with the
     * given chunk.
     * @param c chunk
     */
    void replaceChunk(const Chunk& c);

    /**
     * Reads the actual chunk from its index in the file.
     *
     * @param index the file chunk index
     * @return the real chunk
     */
    Chunk readChunkFromFiles(size_t index);

    /**
     * Writes the actual chunk to the file, at the optional given index, or at
     * the end of the file if none given.
     *
     * @param c the chunk to write
     * @param index the index to write the chunk to
     */
    void writeChunkToFiles(const Chunk& c, size_t index);

    /**
     * Initializes the geometry members in the metadata according to the
     * actual file size.
     *
     * @param filename file name
     * @param fileSize file size to set
     * @param chunkSize chunk size to set
     */
    void createDefaultGeometry(const std::string& filename, size_t fileSize,
    		size_t chunkSize);

    /**
     * Initializes the functionals to the identity matrix.
     */
    void createIdentityMatrix();

    /**
     * Initializes the functionals to an empty matrix.
     */
    void createEmptyMatrix();

    /**
     * Calculates the index in the file where the given chunk functional should
     * go. The actual calculation is done according to the amount of leading
     * zeros in the functional.
     * A functional with N leading zeros means the index is N.
     *
     * @param functional the functional of the chunk to check
     * @return the index where the chunk should go in the file
     */
    size_t calculateFunctionalIndex(const BitVector& functional);

    /**
     * Calculates what files and offsets are required to read or write the chunk
     * at the given index.
     *
     * @param index the index of the chunk
     * @return a vector of all the file positions that the chunk contains
     */
    std::vector<MetaData::FileData> calculateFilesForIndex(size_t index);

    /**
     * Convenience function: return a reference to the functional matrix.
     */
    std::vector<BitVector>& funcs();

private:
    /** The files we are working with */
    std::vector<boost::filesystem::fstream*> m_files;
    /** The meta-data information file */
    MetaFile* m_metafile;
    /** The meta-data itself */
    MetaData* m_metadata;
}; // class FileObject

} // namespace BT2

#endif

