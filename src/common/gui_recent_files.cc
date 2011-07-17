/*
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** 
 * @file common/gui_recent_files.cc
 *
 * @author Kai Sterker
 * @brief Keep track of recently opened files.
 */

#include "util.h"
#include "gui_recent_files.h"

static void on_recent_file_selected (GtkRecentChooser *chooser, gpointer user_data)
{
    GuiRecentFiles *recent_files = (GuiRecentFiles *) user_data;
    GuiRecentFileListener *listener = recent_files->getListener();

    if (listener != NULL)
    {
        gchar* uri = gtk_recent_chooser_get_current_uri (chooser);
        gchar* file = g_filename_from_uri (uri, NULL, NULL);

        listener->OnRecentFileActivated(file);

        g_free (file);
        g_free (uri);
    }
}

// ctor
GuiRecentFiles::GuiRecentFiles(const std::string & cmd, const std::string & args)
{
    Listener = NULL;
    Command = cmd;
    Arguments = args;
    Manager = gtk_recent_manager_get_default ();

    // only show what's been opened by the current program
    GtkRecentFilter *filter = gtk_recent_filter_new ();
    gtk_recent_filter_add_application (filter, cmd.c_str());

    // create menu to use in application
    Menu = gtk_recent_chooser_menu_new_for_manager (Manager);
    gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (Menu), filter);
    gtk_recent_chooser_set_limit (GTK_RECENT_CHOOSER (Menu), 10);
    gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (Menu), GTK_RECENT_SORT_MRU);

    g_signal_connect (G_OBJECT(Menu), "item-activated", G_CALLBACK (on_recent_file_selected), this);
}

// dtor
GuiRecentFiles::~GuiRecentFiles()
{
}

// register new recent file
void GuiRecentFiles::registerFile (const std::string & file, const std::string & type)
{
    GtkRecentData info;
    info.display_name = NULL;
    info.description = NULL;
    info.mime_type = g_strdup (type.c_str());
    info.app_name = g_strdup (Command.c_str());
    info.app_exec = g_strdup (Arguments.c_str());
    info.groups = NULL;
    info.is_private = FALSE;

    gchar* uri = g_filename_to_uri (util::get_absolute_path(file).c_str(), NULL, NULL);
    gtk_recent_manager_add_full (Manager, uri, &info);
}
