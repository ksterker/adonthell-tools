/*
   $Id: gui_file.cc,v 1.2 2004/08/02 07:39:24 ksterker Exp $

   Copyright (C) 2002/2004 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Dlgedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dlgedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file gui_file.cc
 *
 * @author Kai Sterker
 * @brief Wrapper for the GtkFileChooser widget
 */

#include <gtk/gtk.h>
#include "cfg_data.h"
#include "gui_file.h"
#include "gui_dlgedit.h"

// create a new file selection window
GuiFile::GuiFile (GtkFileChooserAction action, const std::string &title, const std::string &file) : GuiModalDialog ()
{
    gchar *label;
    
    switch (action)
    {
        case GTK_FILE_CHOOSER_ACTION_SAVE:
        {
            label = GTK_STOCK_SAVE;
            break;
        }
        case GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER:
        {
            label = GTK_STOCK_NEW;
            break;
        }
        default: label = GTK_STOCK_OPEN;
    }
    
    GtkWindow *parent = (GtkWindow *) GuiDlgedit::window->getWindow ();
    window = gtk_file_chooser_dialog_new (title.c_str (), parent, action,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, label, GTK_RESPONSE_ACCEPT, NULL);
    
    // forbid multiple selections
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(window), FALSE);
    
    // set current directory and file
    gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(window), file.c_str ());
    if (action == GTK_FILE_CHOOSER_ACTION_SAVE || action == GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER)
    {
        gchar *name = g_path_get_basename (file.c_str ());
        gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(window), name);
        g_free (name);
    }
    
    // set shortcuts
    const std::vector<std::string> & projects = CfgData::data->projectsFromDatadir ();
    for (std::vector<std::string>::const_iterator i = projects.begin (); i != projects.end (); i++)
    {
        const std::string &dir = CfgData::data->getBasedir (*i);
        if (dir.length () > 0) 
            gtk_file_chooser_add_shortcut_folder (GTK_FILE_CHOOSER(window), dir.c_str (), NULL);
    }
}

// clean up
GuiFile::~GuiFile ()
{
}

// display file select widget
bool GuiFile::run ()
{
    if (gtk_dialog_run (GTK_DIALOG (window)) == GTK_RESPONSE_ACCEPT)
    {
        gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (window));
        File = filename;
        g_free (filename);
        
        return true;
    }
    
    return false;
}

void GuiFile::add_filter (const std::string & pattern, const std::string & name)
{
    GtkFileFilter *filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter, name.c_str ());
    gtk_file_filter_add_pattern (filter, pattern.c_str ());
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER(window), filter);    
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(window), filter);
}
