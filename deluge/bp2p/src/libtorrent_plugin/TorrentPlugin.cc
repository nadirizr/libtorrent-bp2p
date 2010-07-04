#include "TorrentPlugin.h"

#include <betterp2p/common/utility.h>
#include <betterp2p/control/FileHandler.h>

#include <libtorrent/peer_connection.hpp>
#include <libtorrent/bt_peer_connection.hpp>

#include <iostream>
using namespace std;


namespace BT2 {


boost::shared_ptr<libtorrent::torrent_plugin> create_bp2p_plugin(libtorrent::torrent* t, void* d)
{
    cerr << "// create_bp2p_plugin: creating for torrent '" << t->name() << "'" << endl;
    TorrentPlugin* tp = new TorrentPlugin(*t);

    // let our storage have access to the info hashes
    ((StorageImpl*)t->get_storage())->setTorrentPlugin(tp);
    // create our extension
    return boost::shared_ptr<libtorrent::torrent_plugin>(tp);
}

TorrentPlugin::TorrentPlugin(libtorrent::torrent& t)
    : m_torrent(t), m_storage((StorageImpl&)*(t.get_storage())),
      m_torrent_marker(getRandom())
{ }

boost::shared_ptr<libtorrent::peer_plugin> TorrentPlugin::new_connection(
        libtorrent::peer_connection* pc)
{
    return boost::shared_ptr<libtorrent::peer_plugin>(
            new PeerPlugin(m_torrent, *pc, *this));
}

void TorrentPlugin::tick()
{
    m_storage.tick();
}

bool TorrentPlugin::on_piece_passed_check(int index)
{
    // if the chunk is written, and it passed a hash check, mark it as verified
    if (m_storage.m_slots_written[index] == StorageImpl::SLOT_WRITTEN) {
        m_storage.m_slots_written[index] = StorageImpl::SLOT_VERIFIED;
    }

    // at any rate, do not stop libtorrent
    return false;
}

bool TorrentPlugin::on_piece_failed_check(int index)
{
    // if this chunk is free, it can only be because it is our chunk
    // and we have checked it's functional - if so, bail out our peer
    if (m_storage.m_slots_written[index] == StorageImpl::SLOT_FREE) {
        // first clear the piece
        m_torrent.picker().restore_piece(index);
        m_torrent.restore_piece_state(index);

        // then stop the rest of the process
        return true;
    }

    // in any other case, let libtorrent do it's thing
    return false;
}


const std::string PeerPlugin::EXTENSION_NAME("BP2P");

void PeerPlugin::add_handshake(libtorrent::entry& e)
{
    libtorrent::entry& messages = e["m"];
    messages[EXTENSION_NAME.c_str()] = EXTENSION_STATUS_BP2P;
}

bool PeerPlugin::on_extension_handshake(libtorrent::lazy_entry const& e) 
{
    m_extension_status = EXTENSION_STATUS_NORMAL;
    
    // extract the BP2P status
    if (e.type() != libtorrent::lazy_entry::dict_t) return false;
    libtorrent::lazy_entry const* messages = e.dict_find("m");
    if (!messages || messages->type() != libtorrent::lazy_entry::dict_t) return false;

    int ext_status = messages->dict_find_int_value(EXTENSION_NAME.c_str(), -1);
    if (ext_status == -1) return false;

    // set the peer to be BP2P peer
    m_extension_status = (extension_status)ext_status;

    // if this is a BP2P peer, make sure it prefers to download whole chunks
    if (m_extension_status == EXTENSION_STATUS_BP2P) {
        m_peer_connection.prefer_whole_pieces(1);
    }

    // no other handlers need to handle this message
    return true;
}

bool PeerPlugin::on_request(libtorrent::peer_request const& req) 
{
    // if this is a 'bp2p' peer, return our special chunk and send a header
    if (m_extension_status == EXTENSION_STATUS_BP2P) {
        cerr << "// PeerPlugin::on_request: BP2P peer requesting chunk (" << req.piece << ")" << endl;

        // if this is the last piece in the torrent (and it is probably shorter
        // than the rest), send it normally
        if (req.piece == (m_torrent.torrent_file().num_pieces() - 1)) {
            return false;
        }
    }
    else { // if this is a normal peer
        // mark the piece to read it as a regular chunk
        cerr << "// PeerPlugin::on_request: REGULAR peer requesting chunk (" << req.piece << ")" << endl;
        m_torrent_plugin.getStorage().markRegularSlot(req.piece);
    }

    // no matter what, let libtorrent continue with any other extension
    return false;
}
    
bool PeerPlugin::on_piece_send(libtorrent::peer_request const& req, libtorrent::disk_buffer_holder& data)
{
    // if this is not our peer, do nothing
    if (m_extension_status != EXTENSION_STATUS_BP2P) return false;

    // read the begining
    std::string start(data.get(), 8);

    // if this is not a BP2P chunk, continue
    if (start.substr(0, StorageImpl::MAGIC_MARKER.size()) != StorageImpl::MAGIC_MARKER) return false;

    cerr << "// PeerPlugin::on_piece_send(" << req.piece << ")" << endl;

    // get the marker
    const char* marker_pointer = start.c_str() + StorageImpl::MAGIC_MARKER.size();
    int marker = (int)libtorrent::detail::read_uint32(marker_pointer);
    cerr << "// --> marker = " << marker << endl;

    // generate the corresponding header
    std::string header = m_torrent_plugin.getStorage().getHeader(marker);
    // if there is no header, we are not supposed to send our message
    if (header.empty()) return false;

    // prepare the header for sending
    cerr << "// --> preparing buffer" << endl;
    libtorrent::buffer::interval ib = m_peer_connection.allocate_send_buffer(10 + header.size());
    libtorrent::detail::write_uint32(6 + header.size(), ib.begin);
    libtorrent::detail::write_uint8(libtorrent::bt_peer_connection::msg_extended, ib.begin);
    libtorrent::detail::write_uint8(m_extension_status, ib.begin);
    libtorrent::detail::write_uint32(marker, ib.begin);
    std::copy(header.begin(), header.end(), ib.begin);
    ib.begin += header.size();
    assert(ib.begin == ib.end);
    cerr << "// --> buffer ready to go" << endl;

    // send it through the peer connection
    m_peer_connection.setup_send();
    cerr << "// --> buffer away..." << endl;

    return false;
}

bool PeerPlugin::on_extended(int length, int msg, libtorrent::buffer::const_interval body) 
{
    // if this is not our message, or this peer does not support BP2P - ignore
    if (msg != EXTENSION_STATUS_BP2P) return false;
    if (m_extension_status != EXTENSION_STATUS_BP2P) return false;

    // if the message is too large, disconnect the peer for misbehaving...
    if (length > 500 * 1024) {
        m_peer_connection.disconnect("bp2p header message larger than 500kB", 2);
        return true;
    }

    // check if the message is ok
    if (body.left() < length) return true;

    // get the marker
    int marker = (int)libtorrent::detail::read_uint32(body.begin);
    // get the header
    std::string header(body.begin, body.left());
    
    cerr << "// PeerPlugin::on_extended: received our header message (marker = " << marker << ")" << endl;

    // feed the header to the storage
    m_torrent_plugin.getStorage().receiveChunk(marker, header);

    return true;
}


} // namespace BT2
