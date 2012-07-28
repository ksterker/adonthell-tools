/*
 Copyright (C) 2010 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file mapedit/gui_grid_dialog.cc
 *
 * @author Kai Sterker
 * @brief Control grid for aligning object placement.
 */

#include <adonthell/world/area_manager.h>

#include "gui_grid_dialog.h"
#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "map_data.h"

// Ui definition
static char grid_dialog_ui[] =
#include "grid-properties.glade.h"
;

static void update_grid (GtkSpinButton *spinbutton, gpointer user_data)
{
    GuiGridDialog* dlg = (GuiGridDialog*) user_data;
    dlg->updateGrid();
}

static void on_auto_adjust (GtkToggleButton *btn, gpointer user_data)
{
    GuiGridDialog* dlg = (GuiGridDialog*) user_data;
    dlg->setAutoAdjust(gtk_toggle_button_get_active (btn));
}

static void on_snap_to_grid (GtkToggleButton *btn, gpointer user_data)
{
    GuiGridDialog* dlg = (GuiGridDialog*) user_data;
    dlg->setSnapToGrid(gtk_toggle_button_get_active (btn));
}

static void on_prevent_overlap (GtkToggleButton *btn, gpointer user_data)
{
    GuiGridDialog* dlg = (GuiGridDialog*) user_data;
    dlg->setPreventOverlap(gtk_toggle_button_get_active (btn));
}

static void on_adjust_grid (GtkButton * button, gpointer user_data)
{
    GuiGrid *grid = (GuiGrid*) user_data;
    MapData *map = (MapData*) world::area_manager::get_map();
    
    // update grid from selected object
    grid->grid_from_cur_object (map->x(), map->y());
    grid->draw ();
    
    // redraw mapview with changed grid
    GuiMapedit::window->view()->draw();
}

static void on_align_object (GtkToggleButton *button, gpointer user_data)
{
    GuiGridDialog* dlg = (GuiGridDialog*) user_data;
    gchar *counterpart = (gchar*) g_object_get_data(G_OBJECT(button), "counterpart");
    dlg->updateAlignment(gtk_toggle_button_get_active (button), counterpart);
}
    
static void on_close (GtkDialog *widget, gpointer user_data)
{
    GuiGridDialog* dlg = (GuiGridDialog*) user_data;
    dlg->close();
}

// ctor
GuiGridDialog::GuiGridDialog (GuiGrid *grid, GtkWidget* ctrl) 
: GuiModalDialog (GTK_WINDOW(GuiMapedit::window->getWindow()), false)
{
    GError *err = NULL;
    GObject *widget;
    GtkAdjustment *adj;
    
    Grid = grid;
    GridControl = ctrl;
    GridChanging = false;
    
    Ui = gtk_builder_new();
    
	if (!gtk_builder_add_from_string(Ui, grid_dialog_ui, -1, &err)) 
    {
        g_message ("building entity dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get reference to dialog window
    widget = gtk_builder_get_object (Ui, "grid-dialog");
    window = GTK_WIDGET (widget);
    g_signal_connect (widget, "close", G_CALLBACK (on_close), this);
    
    // initialize values and connect callbacks
    widget = gtk_builder_get_object (Ui, "x_offset");
    adj = (GtkAdjustment *) gtk_adjustment_new (Grid->Ox, 0, Grid->Ix-1, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_grid), this);
    
    widget = gtk_builder_get_object (Ui, "y_offset");
    adj = (GtkAdjustment *) gtk_adjustment_new (Grid->Oy, 0, Grid->Iy-1, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_grid), this);    
    
    widget = gtk_builder_get_object (Ui, "x_interval");
    adj = (GtkAdjustment *) gtk_adjustment_new (Grid->Ix, 4, 512, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_grid), this);    
    
    widget = gtk_builder_get_object (Ui, "y_interval");
    adj = (GtkAdjustment *) gtk_adjustment_new (Grid->Iy, 4, 512, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_grid), this);
    
    widget = gtk_builder_get_object (Ui, "cb_autoadjust");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), Grid->AutoAdjust);
    g_signal_connect (widget, "toggled", G_CALLBACK (on_auto_adjust), this);
    
    widget = gtk_builder_get_object (Ui, "cb_enablegrid");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), Grid->SnapToGrid);
    g_signal_connect (widget, "toggled", G_CALLBACK (on_snap_to_grid), this);

    widget = gtk_builder_get_object (Ui, "cb_preventoverlap");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), Grid->PreventOverlap);
    g_signal_connect (widget, "toggled", G_CALLBACK (on_prevent_overlap), this);

    widget = gtk_builder_get_object (Ui, "btn_adjust");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), !Grid->AutoAdjust);
    g_signal_connect (widget, "clicked", G_CALLBACK (on_adjust_grid), Grid);

    // initialize alignments
    if ((Grid->Alignment & GuiGrid::ALIGN_LEFT) == GuiGrid::ALIGN_LEFT)
    {
        widget = gtk_builder_get_object (Ui, "btn_align_left");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), true);
    }
    if ((Grid->Alignment & GuiGrid::ALIGN_RIGHT) == GuiGrid::ALIGN_RIGHT)
    {
        widget = gtk_builder_get_object (Ui, "btn_align_right");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), true);
    }
    if ((Grid->Alignment & GuiGrid::ALIGN_TOP) == GuiGrid::ALIGN_TOP)
    {
        widget = gtk_builder_get_object (Ui, "btn_align_top");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), true);
    }
    if ((Grid->Alignment & GuiGrid::ALIGN_BOTTOM) == GuiGrid::ALIGN_BOTTOM)
    {
        widget = gtk_builder_get_object (Ui, "btn_align_bottom");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), true);
    }    
    
    // connect signals to alignment buttons
    widget = gtk_builder_get_object (Ui, "btn_align_left");
    g_object_set_data (widget, "counterpart", g_strdup ("btn_align_right"));
    g_signal_connect (widget, "toggled", G_CALLBACK (on_align_object), this);

    widget = gtk_builder_get_object (Ui, "btn_align_right");
    g_object_set_data (widget, "counterpart", g_strdup ("btn_align_left"));
    g_signal_connect (widget, "toggled", G_CALLBACK (on_align_object), this);

    widget = gtk_builder_get_object (Ui, "btn_align_top");
    g_object_set_data (widget, "counterpart", g_strdup ("btn_align_bottom"));
    g_signal_connect (widget, "toggled", G_CALLBACK (on_align_object), this);

    widget = gtk_builder_get_object (Ui, "btn_align_bottom");
    g_object_set_data (widget, "counterpart", g_strdup ("btn_align_top"));
    g_signal_connect (widget, "toggled", G_CALLBACK (on_align_object), this);
    
    // listen to grid changes
    Grid->Monitor = this;
    
    // move dialog out of the way
    GtkAllocation size;
    gtk_widget_get_allocation (window, &size);

    int x, y;
    gtk_widget_show_all (window);
    gtk_window_get_position (GTK_WINDOW(window), &x, &y);
    gtk_window_move (GTK_WINDOW(window), x, y - (size.height / 2));
}

// dtor
GuiGridDialog::~GuiGridDialog ()
{
    Grid->Monitor = NULL;
}

// when the dialog is closed
void GuiGridDialog::close ()
{
    gtk_window_present (parent);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(GridControl), false);    
}

// set values after grid has changed
void GuiGridDialog::gridChanged ()
{
    GObject *widget;
    GtkAdjustment *adj;
    GridChanging = true;
    
    widget = gtk_builder_get_object (Ui, "x_offset");
    adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON(widget));
    gtk_adjustment_configure (adj, Grid->Ox, 0, Grid->Ix-1, 1.0, 10.0, 0.0);
    
    widget = gtk_builder_get_object (Ui, "y_offset");
    adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON(widget));
    gtk_adjustment_configure (adj, Grid->Oy, 0, Grid->Iy-1, 1.0, 10.0, 0.0);
    
    widget = gtk_builder_get_object (Ui, "x_interval");
    adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON(widget));
    gtk_adjustment_configure (adj, Grid->Ix, 4, 512, 1.0, 10.0, 0.00);
    
    widget = gtk_builder_get_object (Ui, "y_interval");
    adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON(widget));
    gtk_adjustment_configure (adj, Grid->Iy, 4, 512, 1.0, 10.0, 0.0);
    
    GridChanging = false;
}

void GuiGridDialog::objChanged ()
{
    GObject *widget = gtk_builder_get_object (Ui, "btn_adjust");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), !Grid->AutoAdjust && Grid->CurObject != NULL);
}

// update the grid
void GuiGridDialog::updateGrid ()
{
    if (GridChanging) return;
    
    GObject *widget;
        
    widget = gtk_builder_get_object (Ui, "x_offset");
    s_int32 dx = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget)) - Grid->Ox;
    Grid->Ox = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
    
    widget = gtk_builder_get_object (Ui, "y_offset");
    s_int32 dy = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget)) - Grid->Oy;
    Grid->Oy = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
    
    widget = gtk_builder_get_object (Ui, "x_interval");
    Grid->Ix = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
    
    widget = gtk_builder_get_object (Ui, "y_interval");
    Grid->Iy = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));
    
    // move grid according to new offset
    Grid->scroll (dx, dy);
    
    // redraw mapview with changed grid
    GuiMapedit::window->view()->draw();
}

// update alignment
void GuiGridDialog::updateAlignment (const bool & activated, const gchar *counterpart)
{
    GObject *widget;
    int alignment = 0;
    
    if (activated)
    {
        widget = gtk_builder_get_object (Ui, counterpart);
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), false);
    }
    
    widget = gtk_builder_get_object (Ui, "btn_align_left");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) alignment |= GuiGrid::ALIGN_LEFT;
    widget = gtk_builder_get_object (Ui, "btn_align_right");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) alignment |= GuiGrid::ALIGN_RIGHT;
    widget = gtk_builder_get_object (Ui, "btn_align_top");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) alignment |= GuiGrid::ALIGN_TOP;
    widget = gtk_builder_get_object (Ui, "btn_align_bottom");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) alignment |= GuiGrid::ALIGN_BOTTOM;
    
    Grid->Alignment = alignment;
}

// toggle auto adjust
void GuiGridDialog::setAutoAdjust (const bool & auto_adjust)
{
    Grid->AutoAdjust = auto_adjust;
    
    GObject *widget = gtk_builder_get_object (Ui, "btn_adjust");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), !auto_adjust && Grid->CurObject != NULL);
    
    if (auto_adjust)
    {
        // in autoadjust mode, we always want to align tile top left
        widget = gtk_builder_get_object (Ui, "btn_align_left");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), true);
        widget = gtk_builder_get_object (Ui, "btn_align_top");
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), true);
    }
}

// toggle snap to grid
void GuiGridDialog::setSnapToGrid (const bool & snap_to_grid)
{
    Grid->SnapToGrid = snap_to_grid;
}

// toggle overlap prevention
void GuiGridDialog::setPreventOverlap (const bool & prevent_overlap)
{
    Grid->PreventOverlap = prevent_overlap;
}
