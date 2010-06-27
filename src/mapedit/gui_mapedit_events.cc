/*
   $Id: gui_mapedit_events.cc,v 1.1 2009/03/29 12:27:26 ksterker Exp $
   
   Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Mapedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Mapedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Mapedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file mapedit/gui_mapedit_events.cc
 *
 * @author Kai Sterker
 * @brief The Events triggered via the Map Editor's main window
 */

#include <string>
#include <gtk/gtk.h>

#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_grid.h"
#include "gui_grid_dialog.h"
#include "gui_file.h"

// Main Window: on_widget_destroy App
void on_widget_destroy (GtkWidget * widget, gpointer data)
{
    gtk_main_quit ();
    gtk_widget_destroy (widget);
}

// File Menu: New
void on_file_new_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiMapedit *mapedit = (GuiMapedit *) user_data;
    mapedit->newMap ();
}

 // File Menu: Load
void on_file_load_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiMapedit *mapedit = (GuiMapedit *) user_data;
    GtkWindow *parent = GTK_WINDOW(mapedit->getWindow());
    
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_OPEN, "Load map", mapedit->directory ());
    fs.add_filter ("*.xml", "Adonthell Map");

    // File selection closed with OK
    if (fs.run ()) mapedit->loadMap (fs.getSelection ());
}

/*
// File Menu: Load Recent
void on_file_load_recent_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    // get file
    const char *file = (const char*) gtk_object_get_user_data (GTK_OBJECT (menuitem));

    // load
    GuiMapedit *mapedit = (GuiMapedit *) user_data;
    dlgedit->loadDialogue (file);
}
*/

// File Menu: Save As
void on_file_save_as_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiMapedit *mapedit = (GuiMapedit *) user_data;
    GtkWindow *parent = GTK_WINDOW(mapedit->getWindow());

    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_SAVE, "Save Map", mapedit->filename ());
    fs.add_filter ("*.xml", "Adonthell Map");

    // File selection closed with OK
    if (fs.run ()) mapedit->saveMap (fs.getSelection ());
}

// File Menu: Save
void on_file_save_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiMapedit *mapedit = (GuiMapedit *) user_data;
    std::string filename = mapedit->filename ();

    // only save maps that aren't 'untitled'
    if (filename.find ("untitled") == filename.npos)
        mapedit->saveMap (filename);
    // otherwise open file selection
    else
        on_file_save_as_activate (menuitem, user_data);
}
/*
// File Menu: Revert to Saved
void on_file_revert_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiDlgedit::window->revertDialogue ();
}

// File Menu: Close
void on_file_close_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiDlgedit::window->closeDialogue ();
}
*/

GuiGridDialog *gridCtrl = NULL;

// turn grid on or off
void on_grid_toggled (GtkToggleButton *btn, gpointer user_data)
{
    GuiMapview *view = (GuiMapview*) user_data;
    
    if (gtk_toggle_button_get_active (btn))
    {
        GuiGrid *grid = view->getGrid ();
        gridCtrl = new GuiGridDialog (grid, GTK_WIDGET(btn));
        gridCtrl->run ();
    }
    else
    {
        delete gridCtrl;
        gridCtrl = NULL;
    }
}

// enable or disable zone display
void on_tree_switched (GtkNotebook *nb, GtkNotebookPage *page, guint page_num, gpointer user_data)
{
    GuiMapview *view = (GuiMapview*) user_data;
    view->showZones (page_num == 1);
}

/*
// Display help text associated with a menuitem to the statusbar 
gboolean on_display_help (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
    int id = GPOINTER_TO_INT (gtk_object_get_data (GTK_OBJECT (widget), "help-id"));
    GuiMessages *message = (GuiMessages *) user_data;
    
    message->display (id);
    return FALSE;
}

// Clear text displayed by the statusbar 
gboolean on_clear_help (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
    GuiMessages *message = (GuiMessages *) user_data;
    message->clear ();
    return FALSE;
}
*/
