#
# torrentview.py
#
# Copyright (C) 2007, 2008 Andrew Resch <andrewresch@gmail.com>
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


"""The torrent view component that lists all torrents in the session."""

import pygtk
pygtk.require('2.0')
import gtk, gtk.glade
import gettext
import gobject
from urlparse import urlparse

import deluge.common
import deluge.component as component
from deluge.ui.client import aclient as client
from deluge.log import LOG as log
import deluge.ui.gtkui.listview as listview

# Status icons.. Create them from file only once to avoid constantly
# re-creating them.
icon_downloading = gtk.gdk.pixbuf_new_from_file(
    deluge.common.get_pixmap("downloading16.png"))
icon_seeding = gtk.gdk.pixbuf_new_from_file(
    deluge.common.get_pixmap("seeding16.png"))
icon_inactive = gtk.gdk.pixbuf_new_from_file(
    deluge.common.get_pixmap("inactive16.png"))
icon_alert = gtk.gdk.pixbuf_new_from_file(
    deluge.common.get_pixmap("alert16.png"))
icon_queued = gtk.gdk.pixbuf_new_from_file(
    deluge.common.get_pixmap("queued16.png"))
icon_checking = gtk.gdk.pixbuf_new_from_file(
    deluge.common.get_pixmap("checking16.png"))

# Holds the info for which status icon to display based on state
ICON_STATE = {
    "Allocating": icon_checking,
    "Checking": icon_checking,
    "Downloading": icon_downloading,
    "Seeding": icon_seeding,
    "Paused": icon_inactive,
    "Error": icon_alert,
    "Queued": icon_queued
}

def cell_data_statusicon(column, cell, model, row, data):
    """Display text with an icon"""
    try:
        icon = ICON_STATE[model.get_value(row, data)]
        if cell.get_property("pixbuf") != icon:
            cell.set_property("pixbuf", icon)
    except KeyError:
        pass

def cell_data_progress(column, cell, model, row, data):
    """Display progress bar with text"""
    (value, state_str) = model.get(row, *data)
    if cell.get_property("value") != value:
        cell.set_property("value", value)

    textstr = "%s" % state_str
    if state_str != "Seeding" and value < 100:
        textstr = textstr + " %.2f%%" % value
    if cell.get_property("text") != textstr:
        cell.set_property("text", textstr)

def cell_data_queue(column, cell, model, row, data):
    value = model.get_value(row, data)
    if value < 0:
        cell.set_property("text", "")
    else:
        cell.set_property("text", value + 1)

def queue_column_sort(model, iter1, iter2, data):
    v1 = model[iter1][data]
    v2 = model[iter2][data]
    if v1 == v2:
        return 0
    if v2 < 0:
        return -1
    if v1 < 0:
        return 1
    if v1 > v2:
        return 1
    if v2 > v1:
        return -1

class TorrentView(listview.ListView, component.Component):
    """TorrentView handles the listing of torrents."""
    def __init__(self):
        component.Component.__init__(self, "TorrentView", interval=2000)
        self.window = component.get("MainWindow")
        # Call the ListView constructor
        listview.ListView.__init__(self,
                            self.window.main_glade.get_widget("torrent_view"),
                            "torrentview.state")
        log.debug("TorrentView Init..")

        # This is where status updates are put
        self.status = {}

        # We keep a copy of the previous status to compare for changes
        self.prev_status = {}

        # Register the columns menu with the listview so it gets updated
        # accordingly.
        self.register_checklist_menu(
                            self.window.main_glade.get_widget("menu_columns"))

        # Add the columns to the listview
        self.add_text_column("torrent_id", hidden=True)
        self.add_bool_column("dirty", hidden=True)
        self.add_func_column("#", cell_data_queue, [int], status_field=["queue"], sort_func=queue_column_sort)
        self.add_texticon_column(_("Name"), status_field=["state", "name"],
                                            function=cell_data_statusicon)
        self.add_func_column(_("Size"),
                                            listview.cell_data_size,
                                            [gobject.TYPE_UINT64],
                                            status_field=["total_wanted"])
        self.add_progress_column(_("Progress"),
                                    status_field=["progress", "state"],
                                    col_types=[float, str],
                                    function=cell_data_progress)
        self.add_func_column(_("Seeders"),
                                        listview.cell_data_peer,
                                        [int, int],
                                        status_field=["num_seeds",
                                                        "total_seeds"])
        self.add_func_column(_("Peers"),
                                        listview.cell_data_peer,
                                        [int, int],
                                        status_field=["num_peers",
                                                        "total_peers"])
        self.add_func_column(_("Down Speed"),
                                        listview.cell_data_speed,
                                        [float],
                                        status_field=["download_payload_rate"])
        self.add_func_column(_("Up Speed"),
                                        listview.cell_data_speed,
                                        [float],
                                        status_field=["upload_payload_rate"])
        self.add_func_column(_("ETA"),
                                            listview.cell_data_time,
                                            [int],
                                            status_field=["eta"])
        self.add_func_column(_("Ratio"),
                                            listview.cell_data_ratio,
                                            [float],
                                            status_field=["ratio"])
        self.add_func_column(_("Avail"),
                                            listview.cell_data_ratio,
                                            [float],
                                            status_field=["distributed_copies"])
        self.add_func_column(_("Added"),
                                            listview.cell_data_date,
                                            [float],
                                            status_field=["time_added"])
        self.add_text_column(_("Tracker"), status_field=["tracker_host"])

        # Set filter to None for now
        self.filter = None

        ### Connect Signals ###
        # Connect to the 'button-press-event' to know when to bring up the
        # torrent menu popup.
        self.treeview.connect("button-press-event",
                                    self.on_button_press_event)
        # Connect to the 'changed' event of TreeViewSelection to get selection
        # changes.
        self.treeview.get_selection().connect("changed",
                                    self.on_selection_changed)

        self.treeview.connect("drag-drop", self.on_drag_drop)

    def start(self):
        """Start the torrentview"""
        # We need to get the core session state to know which torrents are in
        # the session so we can add them to our list.
        client.get_session_state(self._on_session_state)

    def _on_session_state(self, state):
        self.treeview.freeze_child_notify()
        model = self.treeview.get_model()
        for torrent_id in state:
            self.add_row(torrent_id, update=False)
            self.mark_dirty(torrent_id)
        self.treeview.set_model(model)
        self.treeview.thaw_child_notify()
        self.update()

    def stop(self):
        """Stops the torrentview"""
        # We need to clear the liststore
        self.liststore.clear()
        self.prev_status = {}

    def shutdown(self):
        """Called when GtkUi is exiting"""
        self.save_state("torrentview.state")

    def set_filter(self, filter_dict):
        """Sets filters for the torrentview..
        see: core.get_torrents_status
        """
        self.filter = dict(filter_dict) #copied version of filter_dict.
        self.update()

    def send_status_request(self, columns=None):
        # Store the 'status_fields' we need to send to core
        status_keys = []
        # Store the actual columns we will be updating
        self.columns_to_update = []

        if columns is None:
            # We need to iterate through all columns
            columns = self.columns.keys()

        # Iterate through supplied list of columns to update
        for column in columns:
            # Make sure column is visible and has 'status_field' set.
            # If not, we can ignore it.
            if self.columns[column].column.get_visible() is True \
                and self.columns[column].hidden is False \
                and self.columns[column].status_field is not None:
                for field in self.columns[column].status_field:
                    status_keys.append(field)
                    self.columns_to_update.append(column)

        # Remove duplicate keys
        self.columns_to_update = list(set(self.columns_to_update))

        # If there is nothing in status_keys then we must not continue
        if status_keys is []:
            return

        # Remove duplicates from status_key list
        status_keys = list(set(status_keys))

        # Request the statuses for all these torrent_ids, this is async so we
        # will deal with the return in a signal callback.
        client.get_torrents_status(
            self._on_get_torrents_status, self.filter, status_keys)

    def update(self):
        # Send a status request
        gobject.idle_add(self.send_status_request)

    def update_view(self, columns=None):
        """Update the view.  If columns is not None, it will attempt to only
        update those columns selected.
        """
        filter_column = self.columns["filter"].column_indices[0]
        # Update the torrent view model with data we've received
        status = self.status

        for row in self.liststore:
            torrent_id = row[self.columns["torrent_id"].column_indices[0]]

            if not torrent_id in status.keys():
                row[filter_column] = False
            else:
                row[filter_column] = True
                if torrent_id in self.prev_status and status[torrent_id] == self.prev_status[torrent_id]:
                    # The status dict is the same, so do not update
                    continue

                # Set values for each column in the row
                for column in self.columns_to_update:
                    column_index = self.get_column_index(column)
                    for i, status_field in enumerate(self.columns[column].status_field):
                        try:
                            # Only update if different
                            row_value = status[torrent_id][status_field]
                            if row[column_index[i]] != row_value:
                                row[column_index[i]] = row_value
                        except Exception, e:
                            log.debug("%s", e)

        # Update the toolbar buttons just in case some state has changed
        component.get("ToolBar").update_buttons()
        component.get("MenuBar").update_menu()

        self.prev_status = status

    def _on_get_torrents_status(self, status):
        """Callback function for get_torrents_status().  'status' should be a
        dictionary of {torrent_id: {key, value}}."""
        self.status = status
        if self.status == self.prev_status and self.prev_status:
            # We do not bother updating since the status hasn't changed
            self.prev_status = self.status
            return
        gobject.idle_add(self.update_view)

    def add_row(self, torrent_id, update=True):
        """Adds a new torrent row to the treeview"""
        # Insert a new row to the liststore
        row = self.liststore.append()
        # Store the torrent id
        self.liststore.set_value(
                    row,
                    self.columns["torrent_id"].column_indices[0],
                    torrent_id)
        if update:
            self.update()

    def remove_row(self, torrent_id):
        """Removes a row with torrent_id"""
        for row in self.liststore:
            if row[self.columns["torrent_id"].column_indices[0]] == torrent_id:
                self.liststore.remove(row.iter)
                # Force an update of the torrentview
                self.update()
                break

    def mark_dirty(self, torrent_id = None):
        for row in self.liststore:
            if not torrent_id or row[self.columns["torrent_id"].column_indices[0]] == torrent_id:
                #log.debug("marking %s dirty", torrent_id)
                row[self.columns["dirty"].column_indices[0]] = True
                if torrent_id: break

    def get_selected_torrent(self):
        """Returns a torrent_id or None.  If multiple torrents are selected,
        it will return the torrent_id of the first one."""
        selected = self.get_selected_torrents()
        if selected:
            return selected[0]
        else:
            return selected

    def get_selected_torrents(self):
        """Returns a list of selected torrents or None"""
        torrent_ids = []
        try:
            paths = self.treeview.get_selection().get_selected_rows()[1]
        except AttributeError:
            # paths is likely None .. so lets return []
            return []
        try:
            for path in paths:
                try:
                    row = self.treeview.get_model().get_iter(path)
                except Exception, e:
                    log.debug("Unable to get iter from path: %s", e)
                    continue

                child_row = self.treeview.get_model().convert_iter_to_child_iter(None, row)
                child_row = self.treeview.get_model().get_model().convert_iter_to_child_iter(child_row)
                if self.liststore.iter_is_valid(child_row):
                    try:
                        value = self.liststore.get_value(child_row, self.columns["torrent_id"].column_indices[0])
                    except Exception, e:
                        log.debug("Unable to get value from row: %s", e)
                    else:
                        torrent_ids.append(value)
            if len(torrent_ids) == 0:
                return []

            return torrent_ids
        except ValueError, TypeError:
            return []

    def get_torrent_status(self, torrent_id):
        """Returns data stored in self.status, it may not be complete"""
        try:
            return self.status[torrent_id]
        except:
            return {}

    def get_visible_torrents(self):
        return self.status.keys()

    ### Callbacks ###
    def on_button_press_event(self, widget, event):
        """This is a callback for showing the right-click context menu."""
        log.debug("on_button_press_event")
        # We only care about right-clicks
        if event.button == 3:
            x, y = event.get_coords()
            path = self.treeview.get_path_at_pos(int(x), int(y))
            if not path:
                return
            row = self.model_filter.get_iter(path[0])

            if self.get_selected_torrents():
                if self.model_filter.get_value(row, self.columns["torrent_id"].column_indices[0]) not in self.get_selected_torrents():
                    self.treeview.get_selection().unselect_all()
                    self.treeview.get_selection().select_iter(row)
            else:
                self.treeview.get_selection().select_iter(row)
            torrentmenu = component.get("MenuBar").torrentmenu
            torrentmenu.popup(None, None, None, event.button, event.time)
            return True

    def on_selection_changed(self, treeselection):
        """This callback is know when the selection has changed."""
        log.debug("on_selection_changed")
        component.get("TorrentDetails").update()
        component.get("ToolBar").update_buttons()
        component.get("MenuBar").update_menu()

    def on_drag_drop(self, widget, drag_context, x, y, timestamp):
        widget.stop_emission("drag-drop")