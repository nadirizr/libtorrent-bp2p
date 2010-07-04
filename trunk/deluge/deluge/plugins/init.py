#
# init.py
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


from deluge.log import LOG as log

class PluginBase:
    def __init__(self):
        self.plugin = None

    def enable(self):
        try:
            log.debug(0)
            if hasattr(self.plugin, "base_enable"):
                log.debug(1)
                self.plugin.base_enable()
                log.debug(2)
            self.plugin.enable()
        except Exception, e:
            log.warning("Unable to enable plugin: %s", e)
        else:
            # If plugin was enabled, call it's update() right away
            self.update()

    def disable(self):
        try:
            if hasattr(self.plugin, "base_disable"):
                self.plugin.base_disable()
            self.plugin.disable()
        except Exception, e:
            log.warning("Unable to disable plugin: %s", e)

    def update(self):
        if hasattr(self.plugin, "update"):
            self.plugin.update()

