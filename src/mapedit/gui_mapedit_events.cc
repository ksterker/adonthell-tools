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

#include "base/base.h"

#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_goto_dialog.h"
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
    fs.add_filter ("*.amap|*.xml", "Adonthell Map");
    fs.add_shortcut (base::Paths().user_data_dir() + "/");

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

    // encourage use of new-style map extension
    std::string filename = mapedit->filename ();
    if (filename.find (".xml", filename.size() - 4) != std::string::npos)
    {
        filename = filename.replace (filename.size() - 3, 3, "amap");
    }

    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_SAVE, "Save Map", filename);
    fs.add_filter ("*.amap", "Adonthell Map");
    fs.add_shortcut (base::Paths().user_data_dir() + "/");

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
void on_tree_switched (GtkNotebook *nb, gpointer page, guint page_num, gpointer user_data)
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

static void activate_sibling_menuitem (GtkMenuItem *item, const char *name, const bool & enable)
{
    GtkMenuShell *menu = GTK_MENU_SHELL (gtk_widget_get_parent (GTK_WIDGET(item)));
    GList *siblings = gtk_container_get_children(GTK_CONTAINER(menu));

    while (siblings->next != NULL)
    {
        if (strcmp (gtk_widget_get_name (GTK_WIDGET (siblings->data)), name) == 0)
        {
            gtk_widget_set_sensitive (GTK_WIDGET (siblings->data), enable);
            return;
        }
        siblings = siblings->next;
    }
}

// View Menu: Zoom In
void on_model_zoom_in (GtkMenuItem * menuitem, gpointer user_data)
{
    if (base::Scale < 4)
    {
        base::Scale++;

        GuiMapedit *mapedit = (GuiMapedit *) user_data;

        activate_sibling_menuitem(menuitem, "item_zoom_out", true);
        activate_sibling_menuitem(menuitem, "item_zoom_in", base::Scale < 4);

        mapedit->view()->zoom();
    }
}

// View Menu: Zoom Out
void on_model_zoom_out (GtkMenuItem * menuitem, gpointer user_data)
{
    if (base::Scale > 1)
    {
        base::Scale--;

        GuiMapedit *mapedit = (GuiMapedit *) user_data;

        activate_sibling_menuitem(menuitem, "item_zoom_out", base::Scale > 1);
        activate_sibling_menuitem(menuitem, "item_zoom_in", true);

        mapedit->view()->zoom();
    }
}

// View Menu: Zoom Normal
void on_model_reset_zoom (GtkMenuItem * menuitem, gpointer user_data)
{
    if (base::Scale != 1)
    {
        base::Scale = 1;

        GuiMapedit *mapedit = (GuiMapedit *) user_data;

        activate_sibling_menuitem(menuitem, "item_zoom_out", false);
        activate_sibling_menuitem(menuitem, "item_zoom_in", true);

        mapedit->view()->zoom();
    }
}

// View Menu: Goto
void on_goto_location (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiGotoDialog dlg;
    dlg.run();
}
