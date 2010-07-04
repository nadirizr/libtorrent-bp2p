#ifndef TORRENT_PLUGIN_H
#define TORRENT_PLUGIN_H


#include "StorageImpl.h"

#include <libtorrent/extensions.hpp>
#include <libtorrent/torrent.hpp>


namespace BT2 {


class TorrentPlugin : public libtorrent::torrent_plugin
{
public:
    TorrentPlugin(libtorrent::torrent& t);
    virtual ~TorrentPlugin() { }
    
    /**
     * For every new peer connection, this method creates our PeerPlugin to
     * support it.
     */
    virtual boost::shared_ptr<libtorrent::peer_plugin> new_connection(
            libtorrent::peer_connection* pc);

    /**
     * Called aproximately once every second by the libtorrent thread.
     */
    virtual void tick();

    /**
     * Called when a piece passes a hash check.
     *
     * We use this function to mark our pieces as verified for the storage.
     */
    virtual bool on_piece_passed_check(int index);

    /**
     * Called when a piece fails a hash check.
     *
     * We use this callback to fool libtorrent into thinking our pieces are ok,
     * even though they obviously do not pass a hash check.
     * This is used to bail out our peers when they send chunks that don't help.
     */
    virtual bool on_piece_failed_check(int index);

    /**
     * Returns the torrent object.
     */
    libtorrent::torrent& getTorrent() { return m_torrent; }

    /**
     * Returns our storage object.
     */
    StorageImpl& getStorage() { return m_storage; }

    /**
     * Generates a unique marker for use in matching headers and data.
     */
    int generateMarker() { return m_torrent_marker++; }

private:
    libtorrent::torrent& m_torrent;
    StorageImpl& m_storage;

    int m_torrent_marker;
};


class PeerPlugin : public libtorrent::peer_plugin
{
public:

    // --- Extension Protocol Constants --- //
    static const std::string EXTENSION_NAME;
    enum extension_status {
        EXTENSION_STATUS_NORMAL = 20,
        EXTENSION_STATUS_BP2P = 21
    };
    // --- Extension Protocol Constants --- //


    PeerPlugin(libtorrent::torrent& t, libtorrent::peer_connection& pc, TorrentPlugin& tp)
        : m_torrent(t), m_peer_connection(pc), m_torrent_plugin(tp) { }
    virtual ~PeerPlugin() { }

    /**
     * Adds our special handshake to make sure this client is the same as us.
     */
    virtual void add_handshake(libtorrent::entry& e);

    /**
     * Checks if the handshake was completed, and classifies this peer as
     * 'regular' or 'bp2p'.
     */
    virtual bool on_extension_handshake(libtorrent::lazy_entry const& e);

    /**
     * Called when we have received a request for a chunk from a peer.
     * 
     * If this is a 'bp2p' peer, we must prepare the storage to send our type
     * of chunk.
     * If this is a 'regular' peer, we must prepare the storage to sebd a
     * regular chunk for this peer.
     */
    virtual bool on_request(libtorrent::peer_request const& req);

    /**
     * Called when we are about to send a chunk to a peer.
     *
     * If this is a 'bp2p' peer, we now have to send the header message via the
     * extension protocol to accompany the real chunk.
     * If this is a 'regular' peer, we do nothing.
     */
    virtual bool on_piece_send(libtorrent::peer_request const& req,
            libtorrent::disk_buffer_holder& data);
    
    /**
     * Called when we have received a message on the extended protocol.
     * 
     * This message should be a header message for a piece we have received.
     * Once we have parsed the message, we must give the data to the storage
     * interface.
     */
    virtual bool on_extended(int length, int msg, libtorrent::buffer::const_interval body);

private:
    libtorrent::torrent& m_torrent;
    libtorrent::peer_connection& m_peer_connection;
    TorrentPlugin& m_torrent_plugin;

    // holds whether this is a BP2P peer or not
    extension_status m_extension_status;
};

boost::shared_ptr<libtorrent::torrent_plugin> create_bp2p_plugin(libtorrent::torrent* t, void*);


}; // namespace BT2


#endif
