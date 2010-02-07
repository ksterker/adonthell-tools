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
 * @file mapedit/gui_zone_dialog.cc
 *
 * @author Kai Sterker
 * @brief View and edit zone properties.
 */

#include <gtk/gtk.h>

#include "gui_mapedit.h"
#include "gui_zone_dialog.h"

// Ui definition
static char edit_zone_ui[] =
#include "zone-properties.glade.h"
;

// close the dialog and keep all changes
static void on_ok_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiZoneDialog *dialog = (GuiZoneDialog *) user_data;
    
    dialog->applyChanges ();
    
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
GuiZoneDialog::GuiZoneDialog (world::zone *z, MapData *map) 
    : GuiModalDialog (GTK_WINDOW(GuiMapedit::window->getWindow()))
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    Zone = z;
    Map = map;
    
	if (!gtk_builder_add_from_string(Ui, edit_zone_ui, -1, &err)) 
    {
        g_message ("building entity dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get reference to dialog window
    window = GTK_WIDGET (gtk_builder_get_object (Ui, "zone_properties"));
    
    // setup callbacks
    widget = gtk_builder_get_object (Ui, "btn_okay");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_ok_button_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_cancel");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_cancel_button_pressed), this);
        
    // disable goto button (until we actually have a use for it)
    widget = gtk_builder_get_object (Ui, "btn_center");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), FALSE);
    
    // set zone id
    widget = gtk_builder_get_object (Ui, "entry_id");
    gtk_entry_set_text (GTK_ENTRY(widget), Zone->name().c_str());
    
    // set zone type
    widget = gtk_builder_get_object (Ui, "cb_meta");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), 
                                  (Zone->type() & world::zone::TYPE_META == world::zone::TYPE_META));
    widget = gtk_builder_get_object (Ui, "cb_render");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), 
                                  (Zone->type() & world::zone::TYPE_RENDER == world::zone::TYPE_RENDER));
    
    // set coordinates
    displayValues ('x');
    displayValues ('y');
    displayValues ('z');
}

// dtor
GuiZoneDialog::~GuiZoneDialog()
{
    // cleanup
    g_object_unref (Ui);
}

// display values for one coordinate
void GuiZoneDialog::displayValues (const char & c)
{
    char tmp[16];
    GObject *widget;
    
    s_int32 min, max, lower, upper;
    switch (c)
    {
        case 'x':
        {
            min = Zone->min().x();
            max = Zone->max().x();
            lower = Map->min().x();
            upper = Map->max().x();
            break;
        }
        case 'y':
        {
            min = Zone->min().y();
            max = Zone->max().y();
            lower = Map->min().y();
            upper = Map->max().y();
            break;
        }
        case 'z':
        {
            min = Zone->min().z();
            max = Zone->max().z();
            lower = Map->min().z();
            upper = Map->max().z();
            break;
        }
    }
    
    sprintf (tmp, "min_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    GtkAdjustment *adj = (GtkAdjustment *) gtk_adjustment_new (min, lower, upper, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);

    sprintf (tmp, "max_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    adj = (GtkAdjustment *) gtk_adjustment_new (max, lower, upper, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    
    sprintf (tmp, "size_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    sprintf (tmp, "%i", max-min);
    gtk_label_set_text (GTK_LABEL(widget), tmp);
}

// "make it so!"
void GuiZoneDialog::applyChanges()
{
    okButtonPressed (true);
}
