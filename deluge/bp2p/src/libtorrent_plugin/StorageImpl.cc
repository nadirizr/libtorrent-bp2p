#include "StorageImpl.h"

#include "../betterp2p/control/FileHandler.h"
#include "../betterp2p/common/Logger.h"
#include "../betterp2p/common/utility.h"
#include "TorrentPlugin.h"

#include <libtorrent/io.hpp>
#include <libtorrent/torrent.hpp>
#include <libtorrent/lazy_entry.hpp>
#include <libtorrent/error_code.hpp>

#include <stdexcept>

#include <iostream>
using namespace std;


namespace BT2 {


const std::string StorageImpl::MAGIC_MARKER = "BP2P";


StorageImpl::~StorageImpl()
{
    delete m_handler;
}

bool StorageImpl::initialize(bool allocate_files)
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "Initializing [allocate_files = " << allocate_files << "]";
    
    // reopen all files
    m_handler->getFileObject().reopenFiles();

    bool ret = m_underlying->initialize(false);
    Logger::getLog("Network").debugStream()
        << "--> Underlying returned '" << ret << "'";
    return ret;
}

bool StorageImpl::has_any_file()
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "has_any_file = " << m_underlying->has_any_file();

    return (m_handler->getMetaData().getFunctionals().size() > 0);
}

int StorageImpl::read(char* buf, int slot, int offset, int size)
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "read(" << slot <<  "," << offset << "," << size << ")";

    // if we have this chunk in our cache, return the next portion
    std::pair<int,int> cache_key(slot, offset);
    if (m_outgoing_data_cache.find(cache_key) != m_outgoing_data_cache.end()) {
        cerr << "// --> found in cache..." << endl;

        // get the data and write it
        ChunkDataEntry& cache_entry = m_outgoing_data_cache[cache_key];
        std::string& cache_data = cache_entry.data;
        assert((offset + size) <= cache_data.size());
        std::copy(cache_data.begin() + offset, cache_data.begin() + offset + size, buf);

        // if needed, reenter the data into the cache
        if ((size_t)(offset + size) < cache_data.size()) {
            cerr << "// --> storing in cache (" << slot << "," << (offset+size) <<")" << endl;
            m_outgoing_data_cache[std::pair<int,int>(slot,offset+size)] = cache_entry;
        }

        // delete the current entry
        m_outgoing_data_cache.erase(cache_key);

        return size;
    }

    if (offset != 0 || size != (int)m_handler->getMetaData().chunkSize) {
        cerr << "// --> read weird chunk: slot = " << slot << " offset = " << offset << " size = " << size << endl;
    }

    // check if this is a BP2P chunk
    // if it is the last chunk, always return a regular one
    // if it is normal_chunk_mode return a regular one
    // if the offset is not 0, and we didn't catch a cached read piece before, always return a regular one
    // if the slot is marked as read normal, return a regular one
    bool is_bp2p_chunk = (!(slot == m_filestorage.num_pieces() - 1) &&
                          (!m_use_normal_chunks) &&
                          (offset == 0) &&
                          (!m_slots_read_normal[slot]));
    if (is_bp2p_chunk) {
        cerr << "// --> is BP2P chunk..." << endl;

        // if this is a chunk that is meant for BP2P, generate our chunk
        boost::recursive_mutex::scoped_lock l(m_outgoing_mutex);

        // generate a marker
        // XXX XXX
        int marker = m_torrent_plugin->generateMarker();
        //int marker = slot;
        // XXX XXX
        
        // get the data and write it
        std::string data = generateChunk(marker, slot);
        std::copy(data.begin(), data.begin() + size, buf);

        // if needed, store the data in the cache for the next block reads
        if ((size_t)size < data.size()) {
            cerr << "// --> storing in cache (" << slot << "," << size <<")" << endl;
            m_outgoing_data_cache[std::pair<int,int>(slot,size)] = ChunkDataEntry("", data, slot);
        }

        return size;
    }
    else {
        cerr << "// --> is regular chunk..." << endl;

        // in case this chunk was marked to read as a regular chunk, reset the flag
        m_slots_read_normal[slot] = false;

        // if this is a normal peer, simply return the chunk data
        std::string data = m_handler->getFileObject().getChunk(slot).getData();
        assert(offset + size <= data.size());
        std::copy(data.begin() + offset, data.begin() + offset + size, buf);
        return size;
    }
}

int StorageImpl::write(const char* buf, int slot, int offset, int size)
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "write(" << slot << "," << offset << "," << size << ")";

    if (offset != 0 || size != (int)m_handler->getMetaData().chunkSize) {
        cerr << "// --> write weird chunk: slot = " << slot << " offset = " << offset << " size = " << size << endl;
    }

    // make sure we are writing the whole piece and not portions
    // in libtorrent, this happens when disk_io_thread::m_coalesce_writes is true
    // it must be true!
    if ((offset != 0) || (size != m_filestorage.piece_size(slot))) {
        m_slots_written[slot] = SLOT_FREE;
        return -1;
    }

    // if our algorithm is done, then this chunk is ok no matter what
    if (m_handler->isDone()) {
        m_slots_written[slot] = SLOT_VERIFIED;
        return size;
    }

    std::string data(buf, size);
    int ret = size;

    // check if this is our kind of chunk
    std::string start(buf, MAGIC_MARKER.length());
    if (start == MAGIC_MARKER) {
        boost::recursive_mutex::scoped_lock l(m_incoming_mutex);

        // read our marker
        const char* data_read_pointer = data.c_str() + MAGIC_MARKER.size();
        int marker = (int)libtorrent::detail::read_uint32(data_read_pointer);

        // try to see if our functional is helpfull, if not toss it out
        size_t functional_size = bitvector_to_string(
                BitVector(m_handler->getMetaData().getMaxChunkCount())).size();
        std::string functional = data.substr(
                MAGIC_MARKER.size() + sizeof(marker), functional_size);
        if (!verifyChunkBlocks(slot, data) || !isHelpfullFunctional(functional)) {
            cerr << "// --> CHUNK WILL NOT BE HELPFULL - dropping it..." << endl;
            m_slots_written[slot] = SLOT_FREE;
            ret = -1;
        }
        else {
            // try to find it in the incoming chunks map
            if (m_incoming_chunk_datas.find(marker) != m_incoming_chunk_datas.end()) {
                // found - insert it into the algorithm
                bool success = insertChunk(slot, marker, m_incoming_chunk_datas[marker].header, data);
                m_incoming_chunk_datas.erase(m_incoming_chunk_datas.find(marker));
                if (!success) {
                    cerr << "// --> CHUNK IS NOT HELPFULL - dropping it..." << endl;
                    m_slots_written[slot] = SLOT_FREE;
                    ret = -1;
                }
            }
            else {
                // not found - put it in the incoming datas map and wait for the header
                m_incoming_chunk_datas[marker] = ChunkDataEntry("", data, slot);
                // anyway, for now mark the in slots_written so that we don't mess
                // with libtorrent
                m_slots_written[slot] = SLOT_VERIFIED;
            }
        }
    }
    else { // this is a normal peer
        // if this is the last chunk, pad the chunk with zeros
        if (slot == m_filestorage.num_pieces() - 1) {
            data.insert(data.end(), m_handler->getMetaData().chunkSize - data.size(), 0);
        }

        assert(data.size() == m_handler->getMetaData().chunkSize);

        // XXX: not sure what is better
        //m_handler->getFileObject().putChunk(Chunk(
        //            slot, m_handler->getMetaData().getMaxChunkCount(), data));
        m_handler->putChunk(Chunk(slot, m_handler->getMetaData().getMaxChunkCount(), data));

        m_slots_written[slot] = SLOT_WRITTEN;
    }

    // if this is the last chunk, we need to verify that all the other chunks
    // are actually here before we seal the deal
    if (m_torrent_plugin->getTorrent().num_have() + 1 == m_filestorage.num_pieces()) {
        cerr << "// --> this is the last chunk to insert..." << endl;
        cerr << "// --> Matrix is:" << endl;
        m_handler->getMetaData().dump();
        
        // check if our algorithm is finished
        if (!m_handler->isDone()) {
            cerr << "// --> algorithm is not done - dropping this chunk..." << endl;
            /*// mark another random chunk as free so that we redownload it
            int other_slot = getRandom() % (m_filestorage.num_pieces() - 1);
            // remove it from libtorrent as well
            removePiece(other_slot);*/

            m_slots_written[slot] = SLOT_FREE;
            ret = -1;
        }
    }

    // if this is an invalid return value, remove the piece
    /*if (ret < 0) {
        removePiece(slot);
    }*/

    return ret;
}

bool StorageImpl::move_storage(libtorrent::fs::path save_path)
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "move_storage(" << m_handler->getMetaData().diskFileName << " -> " << save_path.string() << ")";

    // release the files
    release_files();

    // change paths in our meta-data
    if (m_handler->getMetaData().isSingleFile()) {
        // if this is only one file, change it's path
        m_handler->getMetaData().diskFileName = 
            (save_path / boost::filesystem::path(m_handler->getMetaData().diskFileName).file_string()).string();
    }
    else {
        // if there are multiple files, simply change the main path
        m_handler->getMetaData().diskFileName = save_path.string();
    }

    // move the files in the storage
    bool res = m_underlying->move_storage(save_path);
    if (!res) return res;

    // reopen all of the files
    m_handler->getFileObject().reopenFiles();

    return res;
}

bool StorageImpl::verify_resume_data(libtorrent::lazy_entry const& rd, std::string& error)
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "verify_resume_data()";

    // check if we have the whole file
    if (m_handler->getMetaData().getChunkCount() == m_handler->getMetaData().getMaxChunkCount()) {
        cerr << "// StorageImpl::verify_resume_data: full file - no need for slots_written" << endl;
        return true;
    }

    // check if we have any of the chunks already
    if (m_handler->getMetaData().getFunctionals().size() > 0) {
        // if so, retrieve the downloaded slots list from the resume data
        libtorrent::lazy_entry const* slots_written_str = rd.dict_find_string("slots_written");
        if (!slots_written_str) return false;
        if (slots_written_str->string_length() != m_filestorage.num_pieces()) return false;
        
        m_slots_written = std::vector<char>(
                slots_written_str->string_ptr(),
                slots_written_str->string_ptr() + slots_written_str->string_length());
        cerr << "// StorageImpl::verify_resume_data: loaded slots_written(" << m_slots_written.size() << ")" << endl;
        for (size_t i = 0; i < m_slots_written.size(); ++i) {
            if (m_slots_written[i] != SLOT_FREE &&
                m_slots_written[i] != SLOT_WRITTEN &&
                m_slots_written[i] != SLOT_VERIFIED)
                m_slots_written[i] = SLOT_FREE;
        }

        return true;
    }

    // if none of the conditions are true, then we cannot resume
    return false;
}

bool StorageImpl::write_resume_data(libtorrent::entry& rd) const
{
    boost::recursive_mutex::scoped_lock l(const_cast<StorageImpl*>(this)->m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "write_resume_data()";

    // write the downloaded slot list
    std::string slots_written_str(&m_slots_written[0], m_slots_written.size());
    cerr << "// StorageImpl::write_resume_data: slots_written_str.size() = " << slots_written_str.size() << endl;
    for (size_t i = 0; i < slots_written_str.size(); ++i) {
        if (slots_written_str[i] != SLOT_FREE &&
            slots_written_str[i] != SLOT_WRITTEN &&
            slots_written_str[i] != SLOT_VERIFIED) {
            // if it is an invalid value, simply put a zero
            slots_written_str[i] = SLOT_FREE;
        }
        if (slots_written_str[i] == SLOT_WRITTEN) {
            // if we are not sure about the written slot, don't save it
            slots_written_str[i] = SLOT_FREE;
        }
    }
    rd["slots_written"] = libtorrent::entry(slots_written_str);

    // write the meta file
    m_handler->getFileObject().persist();

    return false;
}

bool StorageImpl::move_slot(int src_slot, int dst_slot)
{
    Logger::getLog("Network").warnStream() 
        << "move_slot(" << src_slot << "," << dst_slot << ")";

    // XXX: not supported by our storage (but doesn't mean much since this is
    // only used in compact mode, which is irrelevant...
    return true;
}

bool StorageImpl::swap_slots(int slot1, int slot2)
{
    Logger::getLog("Network").warnStream() 
        << "swap_slots(" << slot1 << "," << slot2 << ")";

    // XXX: not supported by our storage (but doesn't mean much since this is
    // only used in compact mode, which is irrelevant...
    return true;
}

bool StorageImpl::swap_slots3(int slot1, int slot2, int slot3)
{
    Logger::getLog("Network").warnStream() 
        << "swap_slots3(" << slot1 << "," << slot2 << "," << slot3 <<")";

    // XXX: not supported by our storage (but doesn't mean much since this is
    // only used in compact mode, which is irrelevant...
    return true;
}

libtorrent::sha1_hash StorageImpl::hash_for_slot(int slot, libtorrent::partial_hash& h, int piece_size)
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream() 
        << "hash_for_slot(" << slot << "," << h.offset << "," << piece_size << ")";

    // XXX: this is used to fool libtorrent into behaving the way we want:
    // if the slot number is in our written_slots vector (saved in the resume_data),
    // then a chunk was given under that slot, and it was helpful - so we give
    // it a good hash

    // if we have this slot in the "fool-libtorrent" vector, use it
    if (m_slots_written[slot] == SLOT_VERIFIED) {
        cerr << "// --> found hash in slots_written!" << endl;
        assert(m_torrent_plugin);
        return m_torrent_plugin->getTorrent().torrent_file().hash_for_piece(slot);
    }
    
    cerr << "// --> calculating hash the normal way..." << endl;
    // if not, try to generate the hash in the usual way
    vector<char> buffer(piece_size - h.offset);

    bool current_use_normal_chunks = m_use_normal_chunks;
    m_use_normal_chunks = true;
    read(&buffer[0], slot, h.offset, buffer.size());
    m_use_normal_chunks = current_use_normal_chunks;

    h.h.update(&buffer[0], buffer.size());
    return h.h.final();
}

bool StorageImpl::release_files()
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    if (!m_handler) return false;

    Logger::getLog("Network").debugStream() 
        << "release_files()";

    // close files and change paths in our meta-data
    m_handler->getFileObject().closeFiles();

    // release the files in the storage
    return m_underlying->release_files();
}

bool StorageImpl::rename_file(int index, std::string const& new_filename)
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);

    Logger::getLog("Network").debugStream()
        << "rename_file(" << index << ", " << new_filename << ")";
    
    m_handler->getFileObject().closeFiles();

    if (m_handler->getMetaData().isSingleFile()) {
        // if there is only one file, change it's name
        assert(index == 0);
        m_handler->getMetaData().diskFileName = 
            (boost::filesystem::path(m_handler->getMetaData().diskFileName).parent_path() / new_filename).string();
    }
    else {
        // if there are multiple files, change the one we want
        assert(index >= 0 && index < m_handler->getMetaData().containedFiles.size());
        m_handler->getMetaData().containedFiles[index].path = new_filename;
    }

    // rename the file in the storage
    bool res = m_underlying->rename_file(index, new_filename);
    if (res) return res;

    // reopen the file
    m_handler->getFileObject().reopenFiles();

    return res;
}

bool StorageImpl::delete_files()
{
    boost::recursive_mutex::scoped_lock l(m_main_mutex);
    
    if (!m_handler) return false;

    Logger::getLog("Network").debugStream() 
        << "delete_files()";

    release_files();

    // get the metafile name
    std::string meta_file = MetaFile::getMetaFilePathForFile(m_handler->getMetaData().diskFileName);

    // delete our handlers
    m_handler->getFileObject().closeFiles();
    delete m_handler;
    m_handler = NULL;

    // delete the meta file
    try {
        libtorrent::fs::remove(meta_file);
    }
    catch (boost::system::system_error& e) {
        set_error(meta_file, e.code());
    }

    // delete the files
    return m_underlying->delete_files();
}

void StorageImpl::tick()
{
    boost::recursive_mutex::scoped_lock m(m_main_mutex);

    // go through all of the incoming and outgoing datas and increase TTLs and
    // purge all of the ones that timed out
    {
        boost::recursive_mutex::scoped_lock l(m_outgoing_mutex);
        std::map<int, ChunkDataEntry>::iterator i;
        for (i = m_outgoing_chunk_datas.begin(); i != m_outgoing_chunk_datas.end();) {
            i->second.ttl--;
            if (i->second.ttl <= 0) {
                cerr << "// **** PURGING TIMED OUT: OUTGOING CHUNK DATA" << endl;
                m_outgoing_chunk_datas.erase(i++);
            }
            else {
                ++i;
            }
        }
    }
    {
        boost::recursive_mutex::scoped_lock l(m_incoming_mutex);
        std::map<int, ChunkDataEntry>::iterator i;
        for (i = m_incoming_chunk_datas.begin(); i != m_incoming_chunk_datas.end();) {
            i->second.ttl--;
            if (i->second.ttl <= 0) {
                cerr << "// **** PURGING TIMED OUT: INCOMING CHUNK DATA" << endl;
                m_incoming_chunk_datas.erase(i++);
            }
            else {
                ++i;
            }
        }
    }

    // and now through the cached_data
    {
        boost::recursive_mutex::scoped_lock l(m_outgoing_mutex);
        std::map<std::pair<int,int>, ChunkDataEntry>::iterator i;
        for (i = m_outgoing_data_cache.begin(); i != m_outgoing_data_cache.end();) {
            i->second.ttl--;
            if (i->second.ttl <= 0) {
                cerr << "// **** PURGING TIMED OUT: OUTGOING DATA CACHE" << endl;
                m_outgoing_data_cache.erase(i++);
            }
            else {
                ++i;
            }
        }
    }

    // clear the read cache for libtorrent to produce new chunks for reads
    m_clear_read_cache_counter++;
    if (m_clear_read_cache_counter >= 10) {
        m_torrent_plugin->getTorrent().filesystem().async_clear_read_cache();
        m_clear_read_cache_counter = 0;
    }
}

std::string StorageImpl::getHeader(int marker)
{
    boost::recursive_mutex::scoped_lock m(m_main_mutex);

    boost::recursive_mutex::scoped_lock l(m_outgoing_mutex);

    if (m_outgoing_chunk_datas.find(marker) != m_outgoing_chunk_datas.end()) {
        std::string header = m_outgoing_chunk_datas[marker].header;
        m_outgoing_chunk_datas.erase(m_outgoing_chunk_datas.find(marker));
        return header;
    }
    else{
        return "";
    }
}

void StorageImpl::receiveChunk(int marker, const std::string& header)
{
    boost::recursive_mutex::scoped_lock m(m_main_mutex);

    boost::recursive_mutex::scoped_lock l(m_incoming_mutex);

    cerr << "// StorageImpl::receiveChunk: receiving..." << endl;

    // if the functional isn't helpfull, toss it out
    size_t functional_size = bitvector_to_string(
            BitVector(m_handler->getMetaData().getMaxChunkCount())).size();
    std::string functional = header.substr(
            MAGIC_MARKER.size() + sizeof(marker) + functional_size, functional_size);
    if (!isHelpfullFunctional(functional)) {
        cerr << "// --> CHUNK WILL NOT BE HELPFULL - dropping it..." << endl;
        return;
    }
    
    // if there is already a data string waiting for this marker, get it
    if (m_incoming_chunk_datas.find(marker) != m_incoming_chunk_datas.end()) {
        ChunkDataEntry& entry = m_incoming_chunk_datas[marker];
        insertChunk(entry.slot, marker, header, entry.data);
        m_incoming_chunk_datas.erase(m_incoming_chunk_datas.find(marker));
    }
    else {
        m_incoming_chunk_datas[marker] = ChunkDataEntry(header, "", 0);
    }
}

void StorageImpl::markRegularSlot(int slot)
{
    m_slots_read_normal[slot] = true;
}

void StorageImpl::removePiece(int index)
{
    /*if (m_slots_written[index] == SLOT_FREE) {
        return;
    }*/

    m_torrent_plugin->getTorrent().picker().restore_piece(index);
    m_torrent_plugin->getTorrent().restore_piece_state(index);
    m_slots_written[index] = SLOT_FREE;
}

std::string StorageImpl::generateChunk(int marker, int chunk_num)
{
    // generate a chunk
    // XXX XXX
    Chunk c = m_handler->getChunk();
    cerr << "// generateChunk: FUNC: " << c.getFunctional() << endl;
    cerr << "// generateChunk: slot: " << chunk_num << endl;
    cerr << "// generateChunk: marker: " << marker << endl;
    //Chunk c = m_handler->getFileObject().getChunk(marker);
    // XXX XXX
    cerr << "// generateChunk: chunk generated" << endl;
    std::string functional = bitvector_to_string(c.getFunctional());
    std::string& data = c.getData();

    // prepare the block tags for each block
    // NOTE: this is used to verify that all of the blocks are of the same chunk
    //       for some reason, this is not always true with libtorrent
    int block_size = m_torrent_plugin->getTorrent().block_size();
    int num_of_blocks = std::max(m_filestorage.piece_size(chunk_num) / block_size, 1);
    char block_tag = (char)getRandom();
    std::string block_tags_data(num_of_blocks - 1, '\0');
    for (int i = 0; i < (num_of_blocks - 1); ++i) {
        block_tags_data[i] = data[(i+1) * block_size];
        data[(i+1) * block_size] = block_tag;
    }

    // determine the amount of bytes that need replacing for the header
    size_t replace_bytes_count = MAGIC_MARKER.size() + sizeof(marker) + 1 + functional.size();
    size_t header_size = replace_bytes_count + block_tags_data.size() + functional.length();

    // prepare the header
    std::string header(header_size, '\0');
    std::copy(data.begin(), data.begin() + replace_bytes_count, header.begin()); // write the head replacement bytes
    std::copy(block_tags_data.begin(), block_tags_data.end(), header.begin() + replace_bytes_count); // write the data from the block tags
    std::copy(functional.begin(), functional.end(),
            header.begin() + replace_bytes_count + block_tags_data.size()); // write the functional

    // prepare the data
    std::copy(MAGIC_MARKER.begin(), MAGIC_MARKER.end(), data.begin()); // write the MAGIC_MARKER
    char* data_write_pointer = const_cast<char*>(data.c_str() + MAGIC_MARKER.size());
    libtorrent::detail::write_uint32(marker, data_write_pointer); // write the marker itself
    data[MAGIC_MARKER.size() + sizeof(marker)] = block_tag; // write the block tag byte
    std::copy(functional.begin(), functional.end(),
            data.begin() + MAGIC_MARKER.size() + sizeof(marker) + 1); // write the functional

    // insert the header into the map
    {
        boost::recursive_mutex::scoped_lock l(m_outgoing_mutex);
        m_outgoing_chunk_datas[marker] = ChunkDataEntry(header, "", 0);
    }
    
    cerr << "// generateChunk: done" << endl;
    return data;
}

bool StorageImpl::insertChunk(int slot, int marker, const std::string& header, std::string& data)
{
    cerr << "// StorageImpl::insertChunk(" << slot << "," << marker << "," << header.length() << "," << data.length() << ")" << endl;

    size_t functional_size = bitvector_to_string(
            BitVector(m_handler->getMetaData().getMaxChunkCount())).size();
    size_t replace_bytes_count = MAGIC_MARKER.size() + sizeof(marker) + 1 + functional_size;

    // first, verify that all block tags are the same, and restore them
    char block_tag = data[MAGIC_MARKER.size() + sizeof(marker)];
    int block_size = m_torrent_plugin->getTorrent().block_size();
    int num_of_blocks = std::max(m_filestorage.piece_size(slot) / block_size, 1);
    std::string block_tags_data(header.begin() + replace_bytes_count,
                                header.begin() + replace_bytes_count + (num_of_blocks - 1));
    for (int i = 0; i < (num_of_blocks - 1); ++i) {
        if (data[(i+1) * block_size] != block_tag) {
            cerr << "// --> MISMATCHED BLOCK TAG - discarding chunk altogether!" << endl;
            return false;
        }
        data[(i+1) * block_size] = block_tags_data[i];
    }

    // now, extract both functionals and verify they are the same
    std::string functional(header.begin() + replace_bytes_count + block_tags_data.size(), header.end());
    BitVector bfunctional = bitvector_from_string(functional, m_filestorage.num_pieces());
    std::string data_functional(data.begin() + (replace_bytes_count - functional_size),
                                data.begin() + replace_bytes_count);
    BitVector bdata_functional = bitvector_from_string(data_functional, m_filestorage.num_pieces());
    
    cerr << "// --> HEADER_FUNC: " << bfunctional << endl;
    cerr << "// -->   DATA_FUNC: " << bdata_functional << endl;
    cerr << "// -->        TAPS: " << m_handler->getMetaData().getCompletePieces() << endl;
    if (bfunctional != bdata_functional) {
        cerr << "// VERY BIG PROBLEM: functionals differ..." << endl;
        cerr << "// functional      = " << functional << endl;
        cerr << "// data_functional = " << data_functional << endl;
        return false;
    }

    // restore the data
    std::copy(header.begin(), (header.begin() + replace_bytes_count), data.begin());

    // create the chunk and insert it
    bool was_helpful = m_handler->putChunk(Chunk(bfunctional, data));
    if (was_helpful) {
        cerr << "// --> chunk was helpful - marking slot" << endl;
        // if it was helpful, put the slot in the slots_written array
        m_slots_written[slot] = SLOT_VERIFIED;
    }
    else {
        cerr << "// --> chunk wasn't helpful - dropping it" << endl;
        // if it wasn't helpful, mark it as free in the slots_written
        m_slots_written[slot] = SLOT_FREE;
    }

    return was_helpful;
}

bool StorageImpl::isHelpfullFunctional(const std::string& functional)
{
    return m_handler->getAlgorithm().canAddFunctional(
            bitvector_from_string(functional, m_filestorage.num_pieces()));
}

bool StorageImpl::verifyChunkBlocks(int chunk_num, const std::string& data)
{
    // first, get the block tag from the data
    char block_tag = data[MAGIC_MARKER.size() + sizeof(int)];

    // for each block, the first byte should be the block tag
    int block_size = m_torrent_plugin->getTorrent().block_size();
    int num_of_blocks = std::max(m_filestorage.piece_size(chunk_num) / block_size, 1);
    for (int i = 0; i < (num_of_blocks - 1); ++i) {
        if (data[(i+1) * block_size] != block_tag) {
            cerr << "// --> MISMATCHED BLOCK TAG DETECTED!" << endl;
            return false;
        }
    }

    return true;
}

StorageImpl::StorageImpl(
            libtorrent::file_storage const& storage,
            boost::filesystem::path const& path,
            libtorrent::file_pool& pool,
            libtorrent::storage_interface* underlying)
    : m_handler(NULL), m_torrent_plugin(NULL), m_underlying(underlying),
      m_filestorage(storage), m_filepool(pool), m_slots_written(storage.num_pieces(), 0),
      m_slots_read_normal(storage.num_pieces(), false), m_use_normal_chunks(false),
      m_clear_read_cache_counter(0)
{
    // if we have no underlying storage, create the default one
    if (m_underlying == NULL) {
        m_underlying = libtorrent::default_storage_constructor(
                                storage, path, pool);
    }

    if (m_filestorage.num_files() == 1) { // if this is a single file torrent
        Logger::getLog("Network").debugStream()
            << "Creating handler for torrent '" << storage.name() << "' "
            << "path '" << path.string() << "' "
            << "filename '" << storage.at(0).path << "' "
            << "piece_length " << storage.piece_length() << " "
            << "size " << storage.at(0).size;
        MetaData data(storage.name(), (path / storage.at(0).path).string(), 
                storage.piece_length(), storage.at(0).size);
        m_handler = new FileHandler(data);
    }
    else { // if this is a multi file torrent
        // construct the file data structures for the meta data
        std::vector<MetaData::FileData> files(storage.num_files());
        for (size_t i = 0; i < files.size(); ++i) {
            files[i] = MetaData::FileData(storage.at(i).path.string(), 
                    storage.at(i).offset, storage.at(i).size);
        }
        Logger::getLog("Network").debugStream()
            << "Creating handler for torrent '" << storage.name() << "' "
            << "path '" << path.string() << "' "
            << "num_files " << storage.num_files() << " "
            << "piece_length " << storage.piece_length();
        MetaData data(storage.name(), path.string(), 
                storage.piece_length(), files);
        m_handler = new FileHandler(data);
    }
}


libtorrent::storage_interface* BT2_storage_constructor(
        libtorrent::file_storage const& storage, 
        boost::filesystem::path const& path,
        libtorrent::file_pool& pool,
        libtorrent::storage_interface* underlying_storage)
{
    return new StorageImpl(storage, path, pool, underlying_storage);
}

libtorrent::storage_interface* BT2_storage_constructor(
        libtorrent::file_storage const& storage,
        boost::filesystem::path const& path,
        libtorrent::file_pool& pool)
{
    return new StorageImpl(storage, path, pool);
}


} // namespace BT2
