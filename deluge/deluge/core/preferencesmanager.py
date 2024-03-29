#
# preferencesmanager.py
#
# Copyright (C) 2008 Andrew Resch <andrewresch@gmail.com>
#
# Deluge is free software.
#
# You may redistribute it and/or modify it under the terms of the
# GNU General Public License, as published by the Free Software
# Foundation; either version 3 of the License, or (at your option)
# any later version.
#
# deluge is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with deluge.    If not, write to:
# 	The Free Software Foundation, Inc.,
# 	51 Franklin Street, Fifth Floor
# 	Boston, MA  02110-1301, USA.
#
#    In addition, as a special exception, the copyright holders give
#    permission to link the code of portions of this program with the OpenSSL
#    library.
#    You must obey the GNU General Public License in all respects for all of
#    the code used other than OpenSSL. If you modify file(s) with this
#    exception, you may extend this exception to your version of the file(s),
#    but you are not obligated to do so. If you do not wish to do so, delete
#    this exception statement from your version. If you delete this exception
#    statement from all source files in the program, then also delete it here.
#

#


import os.path
import threading
import gobject

try:
    import deluge.libtorrent as lt
except ImportError:
    import libtorrent as lt
    if not (lt.version_major == 0 and lt.version_minor == 14):
        raise ImportError("This version of Deluge requires libtorrent 0.14!")

import deluge.configmanager
import deluge.common
import deluge.component as component
from deluge.log import LOG as log

DEFAULT_PREFS = {
    "config_location": deluge.configmanager.get_config_dir(),
    "send_info": False,
    "info_sent": 0.0,
    "daemon_port": 58846,
    "allow_remote": False,
    "compact_allocation": False,
    "download_location": deluge.common.get_default_download_dir(),
    "listen_ports": [6881, 6891],
    "copy_torrent_file": False,
    "torrentfiles_location": deluge.common.get_default_download_dir(),
    "plugins_location": os.path.join(deluge.configmanager.get_config_dir(), "plugins"),
    "state_location": os.path.join(deluge.configmanager.get_config_dir(), "state"),
    "prioritize_first_last_pieces": False,
    "random_port": True,
    "dht": True,
    "upnp": True,
    "natpmp": True,
    "utpex": True,
    "lsd": True,
    "enc_in_policy": 1,
    "enc_out_policy": 1,
    "enc_level": 2,
    "enc_prefer_rc4": True,
    "max_connections_global": 200,
    "max_upload_speed": -1.0,
    "max_download_speed": -1.0,
    "max_upload_slots_global": 4,
    "max_half_open_connections": (lambda: deluge.common.windows_check() and
        (lambda: deluge.common.vista_check() and 4 or 8)() or 50)(),
    "max_connections_per_second": 20,
    "ignore_limits_on_local_network": True,
    "max_connections_per_torrent": -1,
    "max_upload_slots_per_torrent": -1,
    "max_upload_speed_per_torrent": -1,
    "max_download_speed_per_torrent": -1,
    "enabled_plugins": [],
    "autoadd_location": deluge.common.get_default_download_dir(),
    "autoadd_enable": False,
    "add_paused": False,
    "max_active_seeding": 5,
    "max_active_downloading": 3,
    "max_active_limit": 8,
    "dont_count_slow_torrents": False,
    "queue_new_to_top": False,
    "stop_seed_at_ratio": False,
    "remove_seed_at_ratio": False,
    "stop_seed_ratio": 2.00,
    "share_ratio_limit": 2.00,
    "seed_time_ratio_limit": 7.00,
    "seed_time_limit": 180,
    "auto_managed": True,
    "move_completed": False,
    "move_completed_path": deluge.common.get_default_download_dir(),
    "new_release_check": True,
    "proxies": {
        "peer": {
            "type": 0,
            "hostname": "",
            "username": "",
            "password": "",
            "port": 8080
        },
        "web_seed": {
            "type": 0,
            "hostname": "",
            "username": "",
            "password": "",
            "port": 8080
        },
        "tracker": {
            "type": 0,
            "hostname": "",
            "username": "",
            "password": "",
            "port": 8080
        },
        "dht": {
            "type": 0,
            "hostname": "",
            "username": "",
            "password": "",
            "port": 8080
        },

    },
    "outgoing_ports": [0, 0],
    "random_outgoing_ports": True,
    "peer_tos": "0x00",
    "rate_limit_ip_overhead": True
}

class PreferencesManager(component.Component):
    def __init__(self):
        component.Component.__init__(self, "PreferencesManager")

        self.config = deluge.configmanager.ConfigManager("core.conf", DEFAULT_PREFS)

    def start(self):
        self.core = component.get("Core")
        self.session = component.get("Core").session
        self.settings = component.get("Core").settings
        self.signals = component.get("SignalManager")

        # Register set functions in the Config
        self.config.register_set_function("torrentfiles_location",
            self._on_set_torrentfiles_location)
        self.config.register_set_function("state_location",
            self._on_set_state_location)
        self.config.register_set_function("listen_ports",
            self._on_set_listen_ports)
        self.config.register_set_function("random_port",
            self._on_set_random_port)
        self.config.register_set_function("outgoing_ports",
            self._on_set_outgoing_ports)
        self.config.register_set_function("random_outgoing_ports",
            self._on_set_random_outgoing_ports)
        self.config.register_set_function("peer_tos",
            self._on_set_peer_tos)
        self.config.register_set_function("dht", self._on_set_dht)
        self.config.register_set_function("upnp", self._on_set_upnp)
        self.config.register_set_function("natpmp", self._on_set_natpmp)
        self.config.register_set_function("utpex", self._on_set_utpex)
        self.config.register_set_function("lsd", self._on_set_lsd)
        self.config.register_set_function("enc_in_policy",
            self._on_set_encryption)
        self.config.register_set_function("enc_out_policy",
            self._on_set_encryption)
        self.config.register_set_function("enc_level",
            self._on_set_encryption)
        self.config.register_set_function("enc_prefer_rc4",
            self._on_set_encryption)
        self.config.register_set_function("max_connections_global",
            self._on_set_max_connections_global)
        self.config.register_set_function("max_upload_speed",
            self._on_set_max_upload_speed)
        self.config.register_set_function("max_download_speed",
            self._on_set_max_download_speed)
        self.config.register_set_function("max_upload_slots_global",
            self._on_set_max_upload_slots_global)
        self.config.register_set_function("max_half_open_connections",
            self._on_set_max_half_open_connections)
        self.config.register_set_function("max_connections_per_second",
            self._on_set_max_connections_per_second)
        self.config.register_set_function("ignore_limits_on_local_network",
            self._on_ignore_limits_on_local_network)
        self.config.register_set_function("share_ratio_limit",
            self._on_set_share_ratio_limit)
        self.config.register_set_function("seed_time_ratio_limit",
            self._on_set_seed_time_ratio_limit)
        self.config.register_set_function("seed_time_limit",
            self._on_set_seed_time_limit)
        self.config.register_set_function("max_active_downloading",
            self._on_set_max_active_downloading)
        self.config.register_set_function("max_active_seeding",
            self._on_set_max_active_seeding)
        self.config.register_set_function("max_active_limit",
            self._on_set_max_active_limit)
        self.config.register_set_function("dont_count_slow_torrents",
            self._on_set_dont_count_slow_torrents)
        self.config.register_set_function("send_info",
            self._on_send_info)
        self.config.register_set_function("proxies",
            self._on_set_proxies)
        self.new_release_timer = None
        self.config.register_set_function("new_release_check",
            self._on_new_release_check)
        self.config.register_set_function("rate_limit_ip_overhead",
            self._on_rate_limit_ip_overhead)

        self.config.register_change_callback(self._on_config_value_change)

    # Config set functions
    def _on_config_value_change(self, key, value):
        self.signals.emit("config_value_changed", key, value)

    def _on_set_torrentfiles_location(self, key, value):
        if self.config["copy_torrent_file"]:
            try:
                os.makedirs(value)
            except Exception, e:
                log.debug("Unable to make directory: %s", e)

    def _on_set_state_location(self, key, value):
        if not os.access(value, os.F_OK):
            try:
                os.makedirs(value)
            except Exception, e:
                log.debug("Unable to make directory: %s", e)

    def _on_set_listen_ports(self, key, value):
        # Only set the listen ports if random_port is not true
        if self.config["random_port"] is not True:
            log.debug("listen port range set to %s-%s", value[0], value[1])
            self.session.listen_on(value[0], value[1])

    def _on_set_random_port(self, key, value):
        log.debug("random port value set to %s", value)
        # We need to check if the value has been changed to true and false
        # and then handle accordingly.
        if value:
            import random
            listen_ports = []
            randrange = lambda: random.randrange(49152, 65525)
            listen_ports.append(randrange())
            listen_ports.append(listen_ports[0]+10)
        else:
            listen_ports = self.config["listen_ports"]

        # Set the listen ports
        log.debug("listen port range set to %s-%s", listen_ports[0],
            listen_ports[1])
        self.session.listen_on(listen_ports[0], listen_ports[1])

    def _on_set_outgoing_ports(self, key, value):
        if not self.config["random_outgoing_ports"]:
            log.debug("outgoing port range set to %s-%s", value[0], value[1])
            self.settings.outgoing_ports = value[0], value[1]
            self.session.set_settings(self.settings)

    def _on_set_random_outgoing_ports(self, key, value):
        if value:
            self.session.outgoing_ports(0, 0)

    def _on_set_peer_tos(self, key, value):
        log.debug("setting peer_tos to: %s", value)
        try:
            self.settings.peer_tos = chr(int(value, 16))
        except ValueError, e:
            log.debug("Invalid tos byte: %s", e)
            return

        self.session.set_settings(self.settings)

    def _on_set_dht(self, key, value):
        log.debug("dht value set to %s", value)
        state_file = deluge.configmanager.get_config_dir("dht.state")
        if value:
            state = None
            try:
                state = lt.bdecode(open(state_file, "rb").read())
            except Exception, e:
                log.warning("Unable to read DHT state file: %s", e)

            try:
                self.session.start_dht(state)
            except Exception, e:
                log.warning("Restoring old DHT state failed: %s", e)
                self.session.start_dht(None)
            self.session.add_dht_router("router.bittorrent.com", 6881)
            self.session.add_dht_router("router.utorrent.com", 6881)
            self.session.add_dht_router("router.bitcomet.com", 6881)
        else:
            self.core.save_dht_state()
            self.session.stop_dht()

    def _on_set_upnp(self, key, value):
        log.debug("upnp value set to %s", value)
        if value:
            self.session.start_upnp()
        else:
            self.session.stop_upnp()

    def _on_set_natpmp(self, key, value):
        log.debug("natpmp value set to %s", value)
        if value:
            self.session.start_natpmp()
        else:
            self.session.stop_natpmp()

    def _on_set_lsd(self, key, value):
        log.debug("lsd value set to %s", value)
        if value:
            self.session.start_lsd()
        else:
            self.session.stop_lsd()

    def _on_set_utpex(self, key, value):
        log.debug("utpex value set to %s", value)
        if value:
            self.session.add_extension(lt.create_ut_pex_plugin)

    def _on_set_encryption(self, key, value):
        log.debug("encryption value %s set to %s..", key, value)
        pe_settings = lt.pe_settings()
        pe_settings.out_enc_policy = \
            lt.enc_policy(self.config["enc_out_policy"])
        pe_settings.in_enc_policy = lt.enc_policy(self.config["enc_in_policy"])
        pe_settings.allowed_enc_level = lt.enc_level(self.config["enc_level"])
        pe_settings.prefer_rc4 = self.config["enc_prefer_rc4"]
        self.session.set_pe_settings(pe_settings)
        set = self.session.get_pe_settings()
        log.debug("encryption settings:\n\t\t\tout_policy: %s\n\t\t\
        in_policy: %s\n\t\t\tlevel: %s\n\t\t\tprefer_rc4: %s",
            set.out_enc_policy,
            set.in_enc_policy,
            set.allowed_enc_level,
            set.prefer_rc4)

    def _on_set_max_connections_global(self, key, value):
        log.debug("max_connections_global set to %s..", value)
        self.session.set_max_connections(value)

    def _on_set_max_upload_speed(self, key, value):
        log.debug("max_upload_speed set to %s..", value)
        # We need to convert Kb/s to B/s
        if value < 0:
            v = -1
        else:
            v = int(value * 1024)

        self.session.set_upload_rate_limit(v)

    def _on_set_max_download_speed(self, key, value):
        log.debug("max_download_speed set to %s..", value)
        # We need to convert Kb/s to B/s
        if value < 0:
            v = -1
        else:
            v = int(value * 1024)
        self.session.set_download_rate_limit(v)

    def _on_set_max_upload_slots_global(self, key, value):
        log.debug("max_upload_slots_global set to %s..", value)
        self.session.set_max_uploads(value)

    def _on_set_max_half_open_connections(self, key, value):
        self.session.set_max_half_open_connections(value)

    def _on_set_max_connections_per_second(self, key, value):
        self.settings.connection_speed = value
        self.session.set_settings(self.settings)

    def _on_ignore_limits_on_local_network(self, key, value):
        self.settings.ignore_limits_on_local_network = value
        self.session.set_settings(self.settings)

    def _on_set_share_ratio_limit(self, key, value):
        log.debug("%s set to %s..", key, value)
        self.settings.share_ratio_limit = value
        self.session.set_settings(self.settings)

    def _on_set_seed_time_ratio_limit(self, key, value):
        log.debug("%s set to %s..", key, value)
        self.settings.seed_time_ratio_limit = value
        self.session.set_settings(self.settings)

    def _on_set_seed_time_limit(self, key, value):
        log.debug("%s set to %s..", key, value)
        # This value is stored in minutes in deluge, but libtorrent wants seconds
        self.settings.seed_time_limit = int(value * 60)
        self.session.set_settings(self.settings)

    def _on_set_max_active_downloading(self, key, value):
        log.debug("%s set to %s..", key, value)
        log.debug("active_downloads: %s", self.settings.active_downloads)
        self.settings.active_downloads = value
        self.session.set_settings(self.settings)

    def _on_set_max_active_seeding(self, key, value):
        log.debug("%s set to %s..", key, value)
        log.debug("active_seeds: %s", self.settings.active_seeds)
        self.settings.active_seeds = value
        self.session.set_settings(self.settings)

    def _on_set_max_active_limit(self, key, value):
        log.debug("%s set to %s..", key, value)
        log.debug("active_limit: %s", self.settings.active_limit)
        self.settings.active_limit = value
        self.session.set_settings(self.settings)

    def _on_set_dont_count_slow_torrents(self, key, value):
        log.debug("%s set to %s..", key, value)
        self.settings.dont_count_slow_torrents = value
        self.session.set_settings(self.settings)

    def _on_send_info(self, key, value):
        log.debug("Sending anonymous stats..")
        """sends anonymous stats home"""
        class Send_Info_Thread(threading.Thread):
            def __init__(self, config):
                self.config = config
                threading.Thread.__init__(self)
            def run(self):
                import time
                now = time.time()
                # check if we've done this within the last week or never
                if (now - self.config["info_sent"]) >= (60 * 60 * 24 * 7):
                    import deluge.common
                    from urllib import quote_plus
                    from urllib2 import urlopen
                    import platform
                    try:
                        url = "http://deluge-torrent.org/stats_get.php?processor=" + \
                            platform.machine() + "&python=" + platform.python_version() \
                            + "&deluge=" + deluge.common.get_version() \
                            + "&os=" + platform.system() \
                            + "&plugins=" + quote_plus(self.config["enabled_plugins"])
                        urlopen(url)
                    except IOError, e:
                        log.debug("Network error while trying to send info: %s", e)
                    else:
                        self.config["info_sent"] = now
        if value:
            Send_Info_Thread(self.config).start()

    def _on_new_release_check(self, key, value):
        if value:
            log.debug("Checking for new release..")
            threading.Thread(target=self.core.get_new_release).start()
            if self.new_release_timer:
                gobject.source_remove(self.new_release_timer)
            # Set a timer to check for a new release every 3 days
            self.new_release_timer = gobject.timeout_add(
                72 * 60 * 60 * 1000, self._on_new_release_check, "new_release_check", True)
        else:
            if self.new_release_timer:
                gobject.source_remove(self.new_release_timer)

    def _on_set_proxies(self, key, value):
        for k, v in value.items():
            if v["type"]:
                proxy_settings = lt.proxy_settings()
                proxy_settings.type = lt.proxy_type(v["type"])
                proxy_settings.username = v["username"]
                proxy_settings.password = v["password"]
                proxy_settings.hostname = v["hostname"]
                proxy_settings.port = v["port"]
                log.debug("setting %s proxy settings", k)
                getattr(self.session, "set_%s_proxy" % k)(proxy_settings)

    def _on_rate_limit_ip_overhead(self, key, value):
        log.debug("%s: %s", key, value)
        self.settings.rate_limit_ip_overhead = value
        self.session.set_settings(self.settings)
