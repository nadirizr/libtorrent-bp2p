#ifndef STORAGE_IMPL_H
#define STORAGE_IMPL_H


#include <libtorrent/storage.hpp>
#include <boost/thread/recursive_mutex.hpp>
#include <vector>


namespace BT2 {


class FileHandler;
class TorrentPlugin;

class StorageImpl : public libtorrent::storage_interface {
public:
    virtual ~StorageImpl();

    /**
     * Create directories and set file sizes, if allocate_files is true. 
     *
     * @param allocate_files true if allocation mode is set to full and sparse
     * files are supported
     * @return true if succeeded, false to indicate error
     */
    virtual bool initialize(bool allocate_files);

    /**
     * Returns true if any of the files have downloaded chunks.
     */
    virtual bool has_any_file();

    /**
     * Reads into buf the given slot, offset and size from the file.
     *
     * @param buf the output buffer to put the data in
     * @param slot the slot from which to read
     * @param offset the offset within the slot data at which to start
     * @param size the amount of bytes to read
     * @return number of bytes, or negative value to indicate an error
     */
    virtual int read(char* buf, int slot, int offset, int size);

    /**
     * Writes the data in buf to the given slot, offset and size in the file.
     *
     * @param buf the input buffer to get the data from
     * @param slot the slot to write to
     * @param offset the offset within the slot data at which to start
     * @param size the amount of bytes to write
     * @return number of bytes, or negative value to indicate an error
     */
    virtual int write(const char* buf, int slot, int offset, int size);

    /**
     * Moves the storage from the current path to the given path.
     *
     * @return non-zero return value indicates an error
     */
    virtual bool move_storage(libtorrent::fs::path save_path);

    /**
     * Verify storage dependent fast resume entries.
     */
    virtual bool verify_resume_data(libtorrent::lazy_entry const& rd, std::string& error);

    /**
     * Write storage dependent fast resume entries.
     */
    virtual bool write_resume_data(libtorrent::entry& rd) const;

    /**
     * Moves (or copies) the content in src_slot to dst_slot.
     */
    virtual bool move_slot(int src_slot, int dst_slot);

    /**
     * Swaps the data in slot1 and slot2.
     */
    virtual bool swap_slots(int slot1, int slot2);

    /**
     * Swaps the puts the data in slot1 in slot2, the data in slot2
     * in slot3 and the data in slot3 in slot1.
     */
    virtual bool swap_slots3(int slot1, int slot2, int slot3);

    /**
     * Returns the sha1-hash for the data at the given slot.
     */
    virtual libtorrent::sha1_hash hash_for_slot(int slot, libtorrent::partial_hash& h, int piece_size);

    /**
     * This will close all open files that are opened for writing.
     * This is called when a torrent has finished downloading.
     * non-zero return value indicates an error.
     */
    virtual bool release_files();

    /**
     * This will rename the file specified by index.
     */
    virtual bool rename_file(int index, std::string const& new_filename);
    
    /**
     * This will close all open files and delete them.
     * non-zero return value indicates an error.
     */
    virtual bool delete_files();

    friend libtorrent::storage_interface* BT2_storage_constructor(
            libtorrent::file_storage const&, 
            boost::filesystem::path const&, 
            libtorrent::file_pool&,
            libtorrent::storage_interface*);
    friend libtorrent::storage_interface* BT2_storage_constructor(
            libtorrent::file_storage const&,
            boost::filesystem::path const&,
            libtorrent::file_pool&);

    /**
     * Returns the used handler.
     *
     * @return the handler
     */
    FileHandler& getHandler() { return *m_handler; }
    
    /**
     * Sets the main torrent object that is used by this storage.
     * It is called by the BP2P extension when it is assigned a torrent.
     *
     * @param t the torrent plugin object
     */
    void setTorrentPlugin(TorrentPlugin* t) { m_torrent_plugin = t; }

    /**
     * This method should be called to perform all periodic operations, which
     * include perging caches and other internal stuff.
     */
    void tick();

    /**
     * After read() has stored a header for the given marker, use the marker to
     * find it and return it.
     *
     * @param marker the marker for the chunk
     * @return the header to send
     */
    std::string getHeader(int marker);

    /**
     * Prepares the storage for a chunk that is to be received (it does not
     * matter if it has arrived or not, it will be flagged).
     * The header from the message sent by the peer that gave us the chunk is
     * given to the method, and the chunk is flagged when it arrives via the
     * write method.
     *
     * @param marker the marker for the chunk
     * @param header the header for the chunk as transmitted by the peer
     */
    void receiveChunk(int marker, const std::string& header);

    /**
     * Marks the given slot number so that the next read from it will be as a
     * regular chunk.
     * NOTE: this only lasts for one read
     *
     * @param slot the slot to mark
     */
    void markRegularSlot(int slot);

    /**
     * Removes the given piece, restoring it's original blank state in
     * libtorrent.
     *
     * @param index the index of the piece to remove
     */
    void removePiece(int index);

    // this is the marker for our chunks
    static const std::string MAGIC_MARKER;

private:
    /**
     * Constructs the StorageImpl object from the parameters given by the
     * session to the object creator.
     */
    StorageImpl(
            libtorrent::file_storage const& storage,
            boost::filesystem::path const& path,
            libtorrent::file_pool& pool,
            libtorrent::storage_interface* underlying = NULL);

    /**
     * Generates a chunk for sending to a peer, returning the data of that
     * chunk to be sent to the peer that requested the chunk.
     * The given marker is then used to identify the chunk when it arrives along
     * with the header to the other peer.
     *
     * @param marker an identifying marker for the chunk
     * @param chunk_num the chunk number for the generated chunk
     * @return the data of the generated chunk
     */
    std::string generateChunk(int marker, int chunk_num);

    /**
     * Inserts the given chunk into the underlying FileHandler object.
     * It first cleans up the data and header, and then creates a chunk for
     * insertion.
     *
     * @return true if the chunk was helpful
     */
    bool insertChunk(int slot, int marker, const std::string& header, std::string& data);

    /**
     * Checks with the algorithm if the functional is helpfull to us.
     *
     * @param functional the functional to check
     * @return true if it is helpfull to us
     */
    bool isHelpfullFunctional(const std::string& functional);

    /**
     * Checks if the blocks in the given chunk of data are consistent.
     * It uses a block tag that should be the same for all blocks.
     *
     * @param chunk_num the number of the chunk
     * @param data the data chunk to check
     * @return true if the chunk has the correct blocks
     */
    bool verifyChunkBlocks(int chunk_num, const std::string& data);

    // the FileHandler that makes it all happen
    FileHandler* m_handler;

    // the main torrent plugin object - originally NULL, and set by extension
    TorrentPlugin* m_torrent_plugin;
    // the underlying storage used for operations like moving the storage
    libtorrent::storage_interface* m_underlying;
    // the file_storage contains access methods to the files
    libtorrent::file_storage const& m_filestorage;
    // the file_pool contains the details of the files we are downloading
    libtorrent::file_pool& m_filepool;

    // saves the state of slots that were written to fool libtorrent with them
    // into thinking we have downloaded chunks
    enum SlotStatus {
        SLOT_FREE = 0,     // slot was not written
        SLOT_WRITTEN = 1,  // slot was written but not confirmed
        SLOT_VERIFIED = 2, // slot was written and confirmed
    };
    std::vector<char> m_slots_written;
    // used to mark slots as normal read
    std::vector<bool> m_slots_read_normal;

    // this struct is used to store chunks in various stages before or during
    // reading or writing
    struct ChunkDataEntry {
        ChunkDataEntry(const std::string& _header = "", const std::string& _data = "",
                int _slot = 0) : header(_header), data(_data), slot(_slot), ttl(1000) { }

        std::string header;
        std::string data;
        int slot;
        int ttl;
    };

    // holds a map from the marker to the outgoing chunk datas
    std::map<int, ChunkDataEntry> m_outgoing_chunk_datas;
    // a mutex for manipulating the outgoing chunks map
    boost::recursive_mutex m_outgoing_mutex;
    
    // holds a map from the marker to the incoming chunk data
    std::map<int, ChunkDataEntry> m_incoming_chunk_datas;
    // a mutex for manipulating the incoming chunks maps
    boost::recursive_mutex m_incoming_mutex;

    // this flag determines if we are distributing our chunks or normal chunks
    bool m_use_normal_chunks;
    // holds a map of data cache for block reading - if reading for some reason
    // occurs in blocks by libtorrent, we store the rest of the read in this map
    // key = (slot, offset), value = ChunkDataEntry
    std::map<std::pair<int,int>, ChunkDataEntry> m_outgoing_data_cache;
    // this counter is used for performing a cache cleanup every 10 ticks
    int m_clear_read_cache_counter;

    // this main mutex is locked for every storage operation
    boost::recursive_mutex m_main_mutex;

    friend class TorrentPlugin;
};


/**
 * Constructs a BT2::StorageImpl storage unit for libtorrent to use.
 * Recieves a storage interface to use for all operations other than read and
 * write.
 * 
 * @param fs the file_storage object to use for filesystem access (in our case
 * we access filesystem directly so this is hardly used)
 * @param path the path into which we should download
 * @param pool the file pool to use (used to manage the individual files)
 * @return the constructed storage_interface
 */
libtorrent::storage_interface* BT2_storage_constructor(
        libtorrent::file_storage const&, 
        boost::filesystem::path const&, 
        libtorrent::file_pool&,
        libtorrent::storage_interface*);

/**
 * Constructs a BT2::StorageImpl storage unit for libtorrent to use.
 * Uses the default storage interface and calls the regular constructor.
 * 
 * @param fs the file_storage object to use for filesystem access (in our case
 * we access filesystem directly so this is hardly used)
 * @param path the path into which we should download
 * @param pool the file pool to use (used to manage the individual files)
 * @return the constructed storage_interface
 */
libtorrent::storage_interface* BT2_storage_constructor(
        libtorrent::file_storage const&, 
        boost::filesystem::path const&, 
        libtorrent::file_pool&);


} // namespace BT2


#endif // STORAGE_IMPL_H

