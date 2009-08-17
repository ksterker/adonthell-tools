/*
   $Id: gui_file.cc,v 1.1 2009/03/29 12:27:25 ksterker Exp $

   Copyright (C) 2002/2004 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   The Adonthell Tools are free software; you can redistribute them and/or modify
   them under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   The Adonthell Tools are distributed in the hope that they will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file common/gui_file.cc
 *
 * @author Kai Sterker
 * @brief Wrapper for the GtkFileChooser widget
 */

#include <gtk/gtk.h>
#include "gui_file.h"

// create a new file selection window
GuiFile::GuiFile (GtkWindow *parent, GtkFileChooserAction action, const std::string &title, const std::string &file) 
: GuiModalDialog (parent)
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
    
    window = gtk_file_chooser_dialog_new (title.c_str (), parent, action,
        GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, label, GTK_RESPONSE_ACCEPT, NULL);
    
    // forbid multiple selections
    gtk_file_chooser_set_select_multiple (GTK_FILE_CHOOSER(window), FALSE);
    
    // set current directory and file
    gtk_file_chooser_set_filename (GTK_FILE_CHOOSER(window), file.c_str ());
    // gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER(window), file.c_str ());
    if (action == GTK_FILE_CHOOSER_ACTION_SAVE || action == GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER)
    {
        gchar *name = g_path_get_basename (file.c_str ());
        gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER(window), name);
        g_free (name);
    }
    
    /*
    FIXME: that uses code from dlgedit. Needs to be more generic.
     
    // set shortcuts
    const std::vector<std::string> & projects = CfgData::data->projectsFromDatadir ();
    for (std::vector<std::string>::const_iterator i = projects.begin (); i != projects.end (); i++)
    {
        const std::string &dir = CfgData::data->getBasedir (*i);
        if (dir.length () > 0) 
            gtk_file_chooser_add_shortcut_folder (GTK_FILE_CHOOSER(window), dir.c_str (), NULL);
    }
    */
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
    unsigned long pos;
    std::string pattern_str = pattern;
    GtkFileFilter *filter = gtk_file_filter_new ();
    gtk_file_filter_set_name (filter, name.c_str ());

    while ((pos = pattern_str.find ('|')) != std::string::npos)
    {
        std::string filter_str = pattern_str.substr (0, pos);
        pattern_str = pattern_str.substr (pos + 1);
        gtk_file_filter_add_pattern (filter, filter_str.c_str ());
    }
    
    gtk_file_filter_add_pattern (filter, pattern_str.c_str ());
    gtk_file_chooser_add_filter (GTK_FILE_CHOOSER(window), filter);    
    gtk_file_chooser_set_filter (GTK_FILE_CHOOSER(window), filter);
}
