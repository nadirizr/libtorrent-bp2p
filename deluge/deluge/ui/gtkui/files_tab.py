#
# files_tab.py
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

import gtk, gtk.glade
import gobject
import gettext
import os.path
import cPickle

from deluge.ui.gtkui.torrentdetails import Tab
from deluge.ui.client import aclient as client
from deluge.configmanager import ConfigManager
import deluge.configmanager
import deluge.component as component
import deluge.common

from deluge.log import LOG as log

def cell_priority(column, cell, model, row, data):
    if model.get_value(row, 5) == -1:
        # This is a folder, so lets just set it blank for now
        cell.set_property("text", "")
        return
    priority = model.get_value(row, data)
    cell.set_property("text", deluge.common.FILE_PRIORITY[priority])

def cell_priority_icon(column, cell, model, row, data):
    if model.get_value(row, 5) == -1:
        # This is a folder, so lets just set it blank for now
        cell.set_property("stock-id", None)
        return
    priority = model.get_value(row, data)
    if deluge.common.FILE_PRIORITY[priority] == "Do Not Download":
        cell.set_property("stock-id", gtk.STOCK_STOP)
    elif deluge.common.FILE_PRIORITY[priority] == "Normal Priority":
        cell.set_property("stock-id", gtk.STOCK_YES)
    elif deluge.common.FILE_PRIORITY[priority] == "High Priority":
        cell.set_property("stock-id", gtk.STOCK_GO_UP)
    elif deluge.common.FILE_PRIORITY[priority] == "Highest Priority":
        cell.set_property("stock-id", gtk.STOCK_GOTO_TOP)

def cell_filename(column, cell, model, row, data):
    """Only show the tail portion of the file path"""
    filepath = model.get_value(row, data)
    cell.set_property("text", os.path.split(filepath)[1])

def cell_progress(column, cell, model, row, data):
    if model.get_value(row, 5) == -1:
    # This is a folder, so lets just set it blank for now
        cell.set_property("visible", False)
        return
    text = model.get_value(row, data[0])
    value = model.get_value(row, data[1])
    cell.set_property("visible", True)
    cell.set_property("text", text)
    cell.set_property("value", value)

class FilesTab(Tab):
    def __init__(self):
        Tab.__init__(self)
        glade = component.get("MainWindow").get_glade()

        self._name = "Files"
        self._child_widget = glade.get_widget("files_tab")
        self._tab_label = glade.get_widget("files_tab_label")

        self.listview = glade.get_widget("files_listview")
        # filename, size, progress string, progress value, priority, file index, icon id
        self.treestore = gtk.TreeStore(str, gobject.TYPE_UINT64, str, int, int, int, str)

        # We need to store the row that's being edited to prevent updating it until
        # it's been done editing
        self._editing_index = None

        # Filename column
        column = gtk.TreeViewColumn(_("Filename"))
        render = gtk.CellRendererPixbuf()
        column.pack_start(render, False)
        column.add_attribute(render, "stock-id", 6)
        render = gtk.CellRendererText()
        render.set_property("editable", True)
        render.connect("edited", self._on_filename_edited)
        render.connect("editing-started", self._on_filename_editing_start)
        render.connect("editing-canceled", self._on_filename_editing_canceled)
        column.pack_start(render, True)
        column.add_attribute(render, "text", 0)
        column.set_sort_column_id(0)
        column.set_clickable(True)
        column.set_resizable(True)
        column.set_expand(False)
        column.set_min_width(200)
        column.set_reorderable(True)
        self.listview.append_column(column)

        # Size column
        column = gtk.TreeViewColumn(_("Size"))
        render = gtk.CellRendererText()
        column.pack_start(render, False)
        column.set_cell_data_func(render, deluge.ui.gtkui.listview.cell_data_size, 1)
        column.set_sort_column_id(1)
        column.set_clickable(True)
        column.set_resizable(True)
        column.set_expand(False)
        column.set_min_width(50)
        column.set_reorderable(True)
        self.listview.append_column(column)

        # Progress column
        column = gtk.TreeViewColumn(_("Progress"))
        render = gtk.CellRendererProgress()
        column.pack_start(render)
        column.set_cell_data_func(render, cell_progress, (2, 3))
        column.set_sort_column_id(3)
        column.set_clickable(True)
        column.set_resizable(True)
        column.set_expand(False)
        column.set_min_width(100)
        column.set_reorderable(True)
        self.listview.append_column(column)

        # Priority column
        column = gtk.TreeViewColumn(_("Priority"))
        render = gtk.CellRendererPixbuf()
        column.pack_start(render, False)
        column.set_cell_data_func(render, cell_priority_icon, 4)
        render = gtk.CellRendererText()
        column.pack_start(render, False)
        column.set_cell_data_func(render, cell_priority, 4)
        column.set_sort_column_id(4)
        column.set_clickable(True)
        column.set_resizable(True)
        column.set_expand(False)
        column.set_min_width(100)
        column.set_reorderable(True)
        self.listview.append_column(column)

        self.listview.set_model(self.treestore)

        self.listview.get_selection().set_mode(gtk.SELECTION_MULTIPLE)

        self.file_menu = glade.get_widget("menu_file_tab")
        self.file_menu_priority_items = [
            glade.get_widget("menuitem_donotdownload"),
            glade.get_widget("menuitem_normal"),
            glade.get_widget("menuitem_high"),
            glade.get_widget("menuitem_highest"),
            glade.get_widget("menuitem_priority_sep")
        ]

        self.listview.connect("row-activated", self._on_row_activated)
        self.listview.connect("button-press-event", self._on_button_press_event)

        self.listview.enable_model_drag_source(
            gtk.gdk.BUTTON1_MASK,
            [('text/plain', 0, 0)],
            gtk.gdk.ACTION_DEFAULT | gtk.gdk.ACTION_MOVE)
        self.listview.enable_model_drag_dest([('text/plain', 0, 0)], gtk.gdk.ACTION_DEFAULT)

        self.listview.connect("drag_data_get", self._on_drag_data_get_data)
        self.listview.connect("drag_data_received", self._on_drag_data_received_data)

        glade.signal_autoconnect({
            "on_menuitem_open_file_activate": self._on_menuitem_open_file_activate,
            "on_menuitem_donotdownload_activate": self._on_menuitem_donotdownload_activate,
            "on_menuitem_normal_activate": self._on_menuitem_normal_activate,
            "on_menuitem_high_activate": self._on_menuitem_high_activate,
            "on_menuitem_highest_activate": self._on_menuitem_highest_activate,
            "on_menuitem_expand_all_activate": self._on_menuitem_expand_all_activate
        })

        # Connect to the 'torrent_file_renamed' signal
        component.get("Signals").connect_to_signal("torrent_file_renamed", self._on_torrent_file_renamed_signal)
        component.get("Signals").connect_to_signal("torrent_folder_renamed", self._on_torrent_folder_renamed_signal)
        component.get("Signals").connect_to_signal("torrent_removed", self._on_torrent_removed_signal)

        # Attempt to load state
        self.load_state()

        # torrent_id: (filepath, size)
        self.files_list = {}

        self.torrent_id = None

    def save_state(self):
        filename = "files_tab.state"
        # Get the current sort order of the view
        column_id, sort_order = self.treestore.get_sort_column_id()

        # Setup state dict
        state = {
            "columns": {},
            "sort_id": column_id,
            "sort_order": int(sort_order) if sort_order else None
        }

        for index, column in enumerate(self.listview.get_columns()):
            state["columns"][column.get_title()] = {
                "position": index,
                "width": column.get_width()
            }

        # Get the config location for saving the state file
        config_location = deluge.configmanager.get_config_dir()

        try:
            log.debug("Saving FilesTab state file: %s", filename)
            state_file = open(os.path.join(config_location, filename), "wb")
            cPickle.dump(state, state_file)
            state_file.close()
        except IOError, e:
            log.warning("Unable to save state file: %s", e)

    def load_state(self):
        filename = "files_tab.state"
        # Get the config location for loading the state file
        config_location = deluge.configmanager.get_config_dir()
        state = None

        try:
            log.debug("Loading FilesTab state file: %s", filename)
            state_file = open(os.path.join(config_location, filename), "rb")
            state = cPickle.load(state_file)
            state_file.close()
        except (EOFError, IOError, AttributeError), e:
            log.warning("Unable to load state file: %s", e)

        if state == None:
            return

        if state["sort_id"] and state["sort_order"] is not None:
            self.treestore.set_sort_column_id(state["sort_id"], state["sort_order"])

        for (index, column) in enumerate(self.listview.get_columns()):
            cname = column.get_title()
            if state["columns"].has_key(cname):
                cstate = state["columns"][cname]
                column.set_sizing(gtk.TREE_VIEW_COLUMN_FIXED)
                column.set_fixed_width(cstate["width"] if cstate["width"] > 0 else 10)
                if state["sort_id"] == index and state["sort_order"] is not None:
                    column.set_sort_indicator(True)
                    column.set_sort_order(state["sort_order"])
                if cstate["position"] != index:
                    # Column is in wrong position
                    if cstate["position"] == 0:
                        self.listview.move_column_after(column, None)
                    elif self.listview.get_columns()[cstate["position"] - 1].get_title() != cname:
                        self.listview.move_column_after(column, self.listview.get_columns()[cstate["position"] - 1])

    def update(self):
        # Get the first selected torrent
        torrent_id = component.get("TorrentView").get_selected_torrents()

        # Only use the first torrent in the list or return if None selected
        if len(torrent_id) != 0:
            torrent_id = torrent_id[0]
        else:
            # No torrent is selected in the torrentview
            self.clear()
            return

        status_keys = ["file_progress", "file_priorities"]
        if torrent_id != self.torrent_id:
            # We only want to do this if the torrent_id has changed
            self.treestore.clear()
            self.torrent_id = torrent_id
            status_keys += ["compact"]

            if self.torrent_id not in self.files_list.keys():
                # We need to get the files list
                log.debug("Getting file list from core..")
                status_keys += ["files"]
            else:
                # We already have the files list stored, so just update the view
                self.update_files()

        client.get_torrent_status(self._on_get_torrent_status, self.torrent_id, status_keys)
        client.force_call(True)

    def clear(self):
        self.treestore.clear()
        self.torrent_id = None

    def _on_row_activated(self, tree, path, view_column):
        if client.is_localhost:
            client.get_torrent_status(self._on_open_file, self.torrent_id, ["save_path", "files"])
            client.force_call(False)

    def get_file_path(self, row, path=""):
        if not row:
            return path

        path = self.treestore.get_value(row, 0) + path
        return self.get_file_path(self.treestore.iter_parent(row), path)

    def _on_open_file(self, status):
        paths = self.listview.get_selection().get_selected_rows()[1]
        selected = []
        for path in paths:
            selected.append(self.treestore.get_iter(path))

        for select in selected:
            path = self.get_file_path(select).split("/")
            filepath = os.path.join(status["save_path"], *path)
            log.debug("Open file '%s'", filepath)
            deluge.common.open_file(filepath)

    ## The following 3 methods create the folder/file view in the treeview
    def prepare_file_store(self, files):
        split_files = { }
        i = 0
        for file in files:
            self.prepare_file(file, file["path"], i, split_files)
            i += 1
        self.add_files(None, split_files)

    def prepare_file(self, file, file_name, file_num, files_storage):
        first_slash_index = file_name.find("/")
        if first_slash_index == -1:
            files_storage[file_name] = (file_num, file)
        else:
            file_name_chunk = file_name[:first_slash_index+1]
            if file_name_chunk not in files_storage:
                files_storage[file_name_chunk] = { }
            self.prepare_file(file, file_name[first_slash_index+1:],
                              file_num, files_storage[file_name_chunk])

    def add_files(self, parent_iter, split_files):
        ret = 0
        for key,value in split_files.iteritems():
            if key.endswith("/"):
                chunk_iter = self.treestore.append(parent_iter,
                                [key, 0, "", 0, 0, -1, gtk.STOCK_DIRECTORY])
                chunk_size = self.add_files(chunk_iter, value)
                self.treestore.set(chunk_iter, 1, chunk_size)
                ret += chunk_size
            else:
                self.treestore.append(parent_iter, [key,
                                        value[1]["size"], "", 0, 0, value[0], gtk.STOCK_FILE])
                ret += value[1]["size"]
        return ret
    ###

    def update_files(self):
        self.treestore.clear()
        self.prepare_file_store(self.files_list[self.torrent_id])
        self.listview.expand_row("0", False)

    def get_selected_files(self):
        """Returns a list of file indexes that are selected"""
        def get_iter_children(itr, selected):
            i = self.treestore.iter_children(itr)
            while i:
                selected.append(self.treestore[i][5])
                if self.treestore.iter_has_child(i):
                    get_iter_children(i, selected)
                i = self.treestore.iter_next(i)

        selected = []
        paths = self.listview.get_selection().get_selected_rows()[1]
        for path in paths:
            i = self.treestore.get_iter(path)
            selected.append(self.treestore[i][5])
            if self.treestore.iter_has_child(i):
                get_iter_children(i, selected)

        return selected

    def get_files_from_tree(self, rows, files_list, indent):
        if not rows:
            return None

        for row in rows:
            if row[5] > -1:
                files_list.append((row[5], row))
            self.get_files_from_tree(row.iterchildren(), files_list, indent+1)
        return None

    def _on_get_torrent_status(self, status):
        # Store this torrent's compact setting
        if "compact" in status:
            self.__compact = status["compact"]

        if "files" in status:
            self.files_list[self.torrent_id] = status["files"]
            self.update_files()

        # (index, iter)
        files_list = []
        self.get_files_from_tree(self.treestore, files_list, 0)
        files_list.sort()
        for index, row in files_list:
            # Do not update a row that is being edited
            if self._editing_index is not None:
                if self._editing_index == row[5]:
                    continue

            progress_string = "%.2f%%" % (status["file_progress"][index] * 100)
            if row[2] != progress_string:
                row[2] = progress_string
            progress_value = status["file_progress"][index] * 100
            if row[3] != progress_value:
                row[3] = progress_value
            file_priority = status["file_priorities"][index]
            if row[4] != file_priority:
                row[4] = file_priority

    def _on_button_press_event(self, widget, event):
        """This is a callback for showing the right-click context menu."""
        log.debug("on_button_press_event")
        # We only care about right-clicks
        if event.button == 3:
            x, y = event.get_coords()
            path = self.listview.get_path_at_pos(int(x), int(y))
            if not path:
                return
            row = self.treestore.get_iter(path[0])

            if self.get_selected_files():
                if self.treestore.get_value(row, 5) not in self.get_selected_files():
                    self.listview.get_selection().unselect_all()
                    self.listview.get_selection().select_iter(row)
            else:
                self.listview.get_selection().select_iter(row)

            for widget in self.file_menu_priority_items:
                widget.set_sensitive(not self.__compact)

            self.file_menu.popup(None, None, None, event.button, event.time)
            return True

    def _on_menuitem_open_file_activate(self, menuitem):
        self._on_row_activated(None, None, None)

    def _set_file_priorities_on_user_change(self, selected, priority):
        """Sets the file priorities in the core.  It will change the selected
            with the 'priority'"""
        file_priorities = []
        def set_file_priority(model, path, iter, data):
            index = model.get_value(iter, 5)
            if index in selected and index != -1:
                file_priorities.append((index, priority))
            elif index != -1:
                file_priorities.append((index, model.get_value(iter, 4)))

        self.treestore.foreach(set_file_priority, None)
        file_priorities.sort()
        priorities = [p[1] for p in file_priorities]
        log.debug("priorities: %s", priorities)

        client.set_torrent_file_priorities(self.torrent_id, priorities)

    def _on_menuitem_donotdownload_activate(self, menuitem):
        self._set_file_priorities_on_user_change(
            self.get_selected_files(),
            deluge.common.FILE_PRIORITY["Do Not Download"])

    def _on_menuitem_normal_activate(self, menuitem):
        self._set_file_priorities_on_user_change(
            self.get_selected_files(),
            deluge.common.FILE_PRIORITY["Normal Priority"])

    def _on_menuitem_high_activate(self, menuitem):
        self._set_file_priorities_on_user_change(
            self.get_selected_files(),
            deluge.common.FILE_PRIORITY["High Priority"])

    def _on_menuitem_highest_activate(self, menuitem):
        self._set_file_priorities_on_user_change(
            self.get_selected_files(),
            deluge.common.FILE_PRIORITY["Highest Priority"])

    def _on_menuitem_expand_all_activate(self, menuitem):
        self.listview.expand_all()

    def _on_filename_edited(self, renderer, path, new_text):
        index = self.treestore[path][5]
        log.debug("new_text: %s", new_text)

        # Don't do anything if the text hasn't changed
        if new_text == self.treestore[path][0]:
            self._editing_index = None
            return

        if index > -1:
            # We are renaming a file
            itr = self.treestore.get_iter(path)
            # Recurse through the treestore to get the actual path of the file
            def get_filepath(i):
                ip = self.treestore.iter_parent(i)
                fp = ""
                while ip:
                    fp = self.treestore[ip][0] + fp
                    ip = self.treestore.iter_parent(ip)
                return fp

            # Only recurse if file is in a folder..
            if self.treestore.iter_parent(itr):
                filepath = get_filepath(itr) + new_text
            else:
                filepath = new_text

            log.debug("filepath: %s", filepath)

            client.rename_files(self.torrent_id, [(index, filepath)])
        else:
            # We are renaming a folder
            folder = self.treestore[path][0]

            parent_path = ""
            itr = self.treestore.iter_parent(self.treestore.get_iter(path))
            while itr:
                parent_path = self.treestore[itr][0] + parent_path
                itr = self.treestore.iter_parent(itr)

            client.rename_folder(self.torrent_id, parent_path + folder, parent_path + new_text)

        self._editing_index = None

    def _on_filename_editing_start(self, renderer, editable, path):
        self._editing_index = self.treestore[path][5]

    def _on_filename_editing_canceled(self, renderer):
        self._editing_index = None

    def _on_torrent_file_renamed_signal(self, torrent_id, index, name):
        log.debug("index: %s name: %s", index, name)
        old_name = self.files_list[torrent_id][index]["path"]
        self.files_list[torrent_id][index]["path"] = name

        # We need to update the filename displayed if we're currently viewing
        # this torrents files.
        if torrent_id == self.torrent_id:
            old_name_len = len(old_name.split("/"))
            name_len = len(name.split("/"))
            if old_name_len != name_len:
                # The parent path list changes depending on which way the file
                # is moving in the tree
                if old_name_len < name_len:
                    parent_path = [o for o in old_name.split("/")[:-1]]
                else:
                    parent_path = [o for o in name.split("/")[:-1]]
                # Find the iter to the parent folder we need to add a new folder
                # to.
                def find_parent(model, path, itr, user_data):
                    if model[itr][0] == parent_path[0] + "/":
                        if len(parent_path) == 1:
                            # This is the parent iter
                            to_create = name.split("/")[len(old_name.split("/")[:-1]):-1]
                            parent_iter = itr

                            for tc in to_create:
                                # We need to check if these folders need to be created
                                child_iter = self.treestore.iter_children(parent_iter)
                                create = True
                                while child_iter:
                                    if self.treestore[child_iter][0] == tc + "/":
                                        create = False
                                        parent_iter = child_iter
                                        break
                                    child_iter = self.treestore.iter_next(child_iter)
                                if create:
                                    parent_iter = self.treestore.append(parent_iter,
                                                [tc + "/", 0, "", 0, 0, -1, gtk.STOCK_DIRECTORY])

                            # Find the iter for the file that needs to be moved
                            def get_file_iter(model, path, itr, user_data):
                                if model[itr][5] == index:
                                    model[itr][0] = name.split("/")[-1]
                                    t = self.treestore.append(
                                        parent_iter,
                                        self.treestore.get(itr,
                                        *xrange(self.treestore.get_n_columns())))
                                    itr_parent = self.treestore.iter_parent(itr)
                                    self.treestore.remove(itr)
                                    self.remove_childless_folders(itr_parent)
                                    return True

                            self.treestore.foreach(get_file_iter, None)
                            return True
                        else:
                            log.debug("parent_path: %s remove: %s", parent_path, model[itr][0])
                            parent_path.remove(model[itr][0][:-1])

                if parent_path:
                    self.treestore.foreach(find_parent, None)
                else:
                    new_folders = name.split("/")[:-1]
                    parent_iter = None
                    for f in new_folders:
                        parent_iter = self.treestore.append(parent_iter,
                                                [f + "/", 0, "", 0, 0, -1, gtk.STOCK_DIRECTORY])
                    child = self.get_iter_at_path(old_name)
                    self.treestore.append(
                        parent_iter,
                        self.treestore.get(child, *xrange(self.treestore.get_n_columns())))
                    self.treestore.remove(child)

            else:
                # This is just changing a filename without any folder changes
                def set_file_name(model, path, itr, user_data):
                    if model[itr][5] == index:
                        model[itr][0] = os.path.split(name)[-1]
                        return True
                self.treestore.foreach(set_file_name, None)

    def get_iter_at_path(self, filepath):
        """
        Returns the gtkTreeIter for filepath
        """
        log.debug("get_iter_at_path: %s", filepath)
        is_dir = False
        if filepath[-1] == "/":
            is_dir = True

        filepath = filepath.split("/")
        if "" in filepath:
            filepath.remove("")

        path_iter = None
        itr = self.treestore.iter_children(None)
        level = 0
        while itr:
            ipath = self.treestore[itr][0]
            if (level + 1) != len(filepath) and ipath == filepath[level] + "/":
                # We're not at the last index, but we do have a match
                itr = self.treestore.iter_children(itr)
                level += 1
                continue
            elif (level + 1) == len(filepath) and ipath == filepath[level] + "/" if is_dir else filepath[level]:
                # This is the iter we've been searching for
                path_iter = itr
                break
            else:
                itr = self.treestore.iter_next(itr)
                continue

        return path_iter

    def reparent_iter(self, itr, parent, move_siblings=False):
        """
        This effectively moves itr plus it's children to be a child of parent

        If move_siblings is True, it will move all itr's siblings to parent
        """
        src = itr
        def move_children(i, dest):
            while i:
                n = self.treestore.append(dest, self.treestore.get(i, *xrange(self.treestore.get_n_columns())))
                to_remove = i
                if self.treestore.iter_children(i):
                    move_children(self.treestore.iter_children(i), n)
                if i != src:
                    i = self.treestore.iter_next(i)
                else:
                    # This is the source iter, we don't want other iters in it's level
                    if not move_siblings:
                        i = None
                self.treestore.remove(to_remove)

        move_children(itr, parent)

    def remove_childless_folders(self, itr):
        """
        Goes up the tree removing childless itrs starting at itr
        """
        while not self.treestore.iter_children(itr):
            parent = self.treestore.iter_parent(itr)
            self.treestore.remove(itr)
            itr = parent

    def _on_torrent_folder_renamed_signal(self, torrent_id, old_folder, new_folder):
        log.debug("on_torrent_folder_renamed_signal")
        log.debug("old_folder: %s new_folder: %s", old_folder, new_folder)

        if old_folder[-1] != "/":
            old_folder += "/"
        if new_folder[-1] != "/":
            new_folder += "/"

        for fd in self.files_list[torrent_id]:
            if fd["path"].startswith(old_folder):
                fd["path"] = fd["path"].replace(old_folder, new_folder, 1)

        if torrent_id == self.torrent_id:

            old_split = old_folder.split("/")
            try:
                old_split.remove("")
            except:
                pass

            new_split = new_folder.split("/")
            try:
                new_split.remove("")
            except:
                pass

            old_folder_iter = self.get_iter_at_path(old_folder)
            old_folder_iter_parent = self.treestore.iter_parent(old_folder_iter)

            new_folder_iter = self.get_iter_at_path(new_folder)
            if len(new_split) == len(old_split):
                # These are at the same tree depth, so it's a simple rename
                self.treestore[old_folder_iter][0] = new_folder
                return
            if new_folder_iter:
                # This means that a folder by this name already exists
                self.reparent_iter(self.treestore.iter_children(old_folder_iter), new_folder_iter)
            else:
                parent = old_folder_iter_parent
                for ns in new_split[:-1]:
                    parent = self.treestore.append(parent, [ns + "/", 0, "", 0, 0, -1, gtk.STOCK_DIRECTORY])

                self.treestore[old_folder_iter][0] = new_split[-1] + "/"
                self.reparent_iter(old_folder_iter, parent)

            # We need to check if the old_folder_iter_parent no longer has children
            # and if so, we delete it
            self.remove_childless_folders(old_folder_iter_parent)

    def _on_torrent_removed_signal(self, torrent_id):
        if torrent_id in self.files_list:
            del self.files_list[torrent_id]

    def _on_drag_data_get_data(self, treeview, context, selection, target_id, etime):
        paths = self.listview.get_selection().get_selected_rows()[1]
        selection.set_text(cPickle.dumps(paths))

    def _on_drag_data_received_data(self, treeview, context, x, y, selection, info, etime):
        selected = cPickle.loads(selection.data)
        log.debug("selection.data: %s", selected)
        drop_info = treeview.get_dest_row_at_pos(x, y)
        model = treeview.get_model()
        if drop_info:
            itr = model.get_iter(drop_info[0])
            parent_iter = model.iter_parent(itr)
            parent_path = ""
            if model[itr][5] == -1:
                parent_path += model[itr][0]

            while parent_iter:
                parent_path = model[parent_iter][0] + parent_path
                parent_iter = model.iter_parent(parent_iter)

            if model[selected[0]][5] == -1:
                log.debug("parent_path: %s", parent_path)
                log.debug("rename_to: %s", parent_path + model[selected[0]][0])
                # Get the full path of the folder we want to rename
                pp = ""
                itr = self.treestore.iter_parent(self.treestore.get_iter(selected[0]))
                while itr:
                    pp = self.treestore[itr][0] + pp
                    itr = self.treestore.iter_parent(itr)
                client.rename_folder(self.torrent_id, pp + model[selected[0]][0], parent_path + model[selected[0]][0])
            else:
                #[(index, filepath), ...]
                to_rename = []
                for s in selected:
                    to_rename.append((model[s][5], parent_path + model[s][0]))
                log.debug("to_rename: %s", to_rename)
                client.rename_files(self.torrent_id, to_rename)
