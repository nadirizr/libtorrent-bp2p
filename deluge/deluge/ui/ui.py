#
# ui.py
#
# Copyright (C) 2007 Andrew Resch <andrewresch@gmail.com>
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

import deluge.configmanager

from deluge.log import LOG as log

DEFAULT_PREFS = {
    "default_ui": "gtk"
}

class UI:
    def __init__(self, options, args, ui_args):
        log.debug("UI init..")

        # Set the config directory
        deluge.configmanager.set_config_dir(options.config)

        config = deluge.configmanager.ConfigManager("ui.conf", DEFAULT_PREFS)

        if not options.ui:
            selected_ui = config["default_ui"]
        else:
            selected_ui = options.ui

        config.save()
        del config

        try:
            if selected_ui == "gtk":
                log.info("Starting GtkUI..")
                from deluge.ui.gtkui.gtkui import GtkUI
                ui = GtkUI(args)
            elif selected_ui == "web":
                log.info("Starting WebUI..")
                from deluge.ui.webui.webui import WebUI
                ui = WebUI(args)
            elif selected_ui == "console":
                log.info("Starting ConsoleUI..")
                from deluge.ui.console.main import ConsoleUI
                ui = ConsoleUI(ui_args).run()
        except ImportError, e:
            import sys
            import traceback
            error_type, error_value, tb = sys.exc_info()
            stack = traceback.extract_tb(tb)
            last_frame = stack[-1]
            if last_frame[0] == __file__:
                log.error("Unable to find the requested UI: %s.  Please select a different UI with the '-u' option or alternatively use the '-s' option to select a different default UI.", selected_ui)
            else:
                log.exception(e)
                log.error("There was an error whilst launching the request UI: %s", selected_ui)
                log.error("Look at the traceback above for more information.")
            sys.exit(1)
