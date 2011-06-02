/* 
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file mapedit/gui_goto_dialog.cc
 *
 * @author Kai Sterker
 * @brief View and edit goto properties.
 */

#include <gtk/gtk.h>
#include <base/base.h>

#include "map_data.h"
#include "map_mgr.h"
#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_goto_dialog.h"

// Ui definition
static char goto_ui[] =
#include "goto.glade.h"
;

// close the dialog and keep all changes
static void on_ok_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiGotoDialog *dialog = (GuiGotoDialog *) user_data;
    dialog->gotoPosition ();
    
    // clean up
    gtk_main_quit ();
}

// close dialog and dismiss all changes
static void on_cancel_button_pressed (GtkButton * button, gpointer user_data)
{
    // clean up
    gtk_main_quit ();
}

// ctor
GuiGotoDialog::GuiGotoDialog ()
    : GuiModalDialog (GTK_WINDOW(GuiMapedit::window->getWindow()))
{
    GtkAdjustment *adj;
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();

	if (!gtk_builder_add_from_string(Ui, goto_ui, -1, &err))
    {
        g_message ("building entity dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get reference to dialog window
    window = GTK_WIDGET (gtk_builder_get_object (Ui, "goto-dialog"));
    
    // setup callbacks
    widget = gtk_builder_get_object (Ui, "btn_ok");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_ok_button_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_close");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_cancel_button_pressed), this);
        
    // the current map
    MapData *map = (MapData*) MapMgr::get_map();
    char tmp[32];

    // set min and max coordinates of the map
    sprintf (tmp, "%i", map->min().x());
    widget = gtk_builder_get_object (Ui, "lbl_min_x");
    gtk_label_set_text (GTK_LABEL(widget), tmp);
    sprintf (tmp, "%i", map->min().y());
    widget = gtk_builder_get_object (Ui, "lbl_min_y");
    gtk_label_set_text (GTK_LABEL(widget), tmp);
    sprintf (tmp, "%i", map->min().z());
    widget = gtk_builder_get_object (Ui, "lbl_min_z");
    gtk_label_set_text (GTK_LABEL(widget), tmp);
    sprintf (tmp, "%i", map->max().x());
    widget = gtk_builder_get_object (Ui, "lbl_max_x");
    gtk_label_set_text (GTK_LABEL(widget), tmp);
    sprintf (tmp, "%i", map->max().y());
    widget = gtk_builder_get_object (Ui, "lbl_max_y");
    gtk_label_set_text (GTK_LABEL(widget), tmp);
    sprintf (tmp, "%i", map->max().z());
    widget = gtk_builder_get_object (Ui, "lbl_max_z");
    gtk_label_set_text (GTK_LABEL(widget), tmp);

    // we want to display the center position
    GtkAllocation allocation;
    gtk_widget_get_allocation (GuiMapedit::window->view()->drawingArea(), &allocation);
    int ox = allocation.width / (2 * base::Scale);
    int oy = allocation.height / (2 * base::Scale);

    // set initial location values
    widget = gtk_builder_get_object (Ui, "location_x");
    adj = (GtkAdjustment *) gtk_adjustment_new (map->x() + ox, map->min().x(), map->max().x(), 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    widget = gtk_builder_get_object (Ui, "location_y");
    adj = (GtkAdjustment *) gtk_adjustment_new (map->y() + oy, map->min().y(), map->max().y(), 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    widget = gtk_builder_get_object (Ui, "location_z");
    adj = (GtkAdjustment *) gtk_adjustment_new (map->z(), map->min().z(), map->max().z(), 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
}

// dtor
GuiGotoDialog::~GuiGotoDialog()
{
    // cleanup
    g_object_unref (Ui);
}

// center map on selected position
void GuiGotoDialog::gotoPosition()
{
    GObject* widget;
    s_int32 x, y, z;

    widget = gtk_builder_get_object (Ui, "location_x");
    x = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
    widget = gtk_builder_get_object (Ui, "location_y");
    y = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
    widget = gtk_builder_get_object (Ui, "location_z");
    z = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));

    GuiMapedit::window->view()->gotoPosition (x, y, z);
    okButtonPressed(true);
}
