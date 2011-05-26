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
#include "gui_mapview.h"
#include "gui_zone.h"
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
    GuiZoneDialog *dialog = (GuiZoneDialog *) user_data;
    dialog->revertChanges ();

    // clean up
    gtk_main_quit ();
}

static void update_size (GtkSpinButton *spinbutton, gpointer user_data)
{
    std::string name (gtk_buildable_get_name (GTK_BUILDABLE (spinbutton)));
    char c = *name.rbegin();

    GuiZoneDialog *dialog = (GuiZoneDialog *) user_data;
    dialog->updateValues (c);
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

    // backup for the zone, in case we cancel editing
    OldZone = new world::zone (z->type(), z->name(), z->min(), z->max());

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
    bool is_active = (Zone->type() & world::zone::TYPE_META) == world::zone::TYPE_META;
    widget = gtk_builder_get_object (Ui, "cb_meta");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), is_active);
    is_active = (Zone->type() & world::zone::TYPE_RENDER) == world::zone::TYPE_RENDER;
    widget = gtk_builder_get_object (Ui, "cb_render");
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(widget), is_active);

    // set coordinates
    displayValues ('x');
    displayValues ('y');
    displayValues ('z');
    
    widget = gtk_builder_get_object (Ui, "min_x");
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_size), this);
    widget = gtk_builder_get_object (Ui, "min_y");
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_size), this);
    widget = gtk_builder_get_object (Ui, "min_z");
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_size), this);
    widget = gtk_builder_get_object (Ui, "size_x");
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_size), this);
    widget = gtk_builder_get_object (Ui, "size_y");
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_size), this);
    widget = gtk_builder_get_object (Ui, "size_z");
    g_signal_connect (widget, "value-changed", G_CALLBACK (update_size), this);

    // update map view
    GuiMapedit::window->view()->getZones()->set_active_zone(Zone);
    GuiMapedit::window->view()->updateOverlay();
    GuiMapedit::window->view()->draw();
}

// dtor
GuiZoneDialog::~GuiZoneDialog()
{
    // cleanup
    g_object_unref (Ui);

    delete OldZone;
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

    sprintf (tmp, "size_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    adj = (GtkAdjustment *) gtk_adjustment_new (max-min, 0, upper-min, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);
    
    sprintf (tmp, "max_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    sprintf (tmp, "%i", max);
    gtk_label_set_text (GTK_LABEL(widget), tmp);
}

// update after a value has been changed
void GuiZoneDialog::updateValues (const char & c)
{
    char tmp[16];
    GObject *widget;

    sprintf (tmp, "min_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    int min = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));

    sprintf (tmp, "size_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    int size = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (widget));

    s_int32 upper;
    switch (c)
    {
        case 'x':
        {
            Zone->min().set_x(min);
            Zone->max().set_x(min + size);

            upper = Map->max().x();
            break;
        }
        case 'y':
        {
            Zone->min().set_y(min);
            Zone->max().set_y(min + size);

            upper = Map->max().y();
            break;
        }
        case 'z':
        {
            Zone->min().set_z(min);
            Zone->max().set_z(min + size);

            upper = Map->max().z();
            break;
        }
    }

    GtkAdjustment *adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (widget));
    gtk_adjustment_set_upper (adj, upper - min);

    sprintf (tmp, "max_%c", c);
    widget = gtk_builder_get_object (Ui, tmp);
    sprintf (tmp, "%i", min+size);
    gtk_label_set_text (GTK_LABEL(widget), tmp);

    // update map view
    GuiMapedit::window->view()->updateOverlay();
    GuiMapedit::window->view()->draw();
}

// discard changes
void GuiZoneDialog::revertChanges()
{
    // update zone
    Zone->set_name(OldZone->name());
    Zone->set_type(OldZone->type());

    Zone->min().set(OldZone->min().x(), OldZone->min().y(), OldZone->min().z());
    Zone->max().set(OldZone->max().x(), OldZone->max().y(), OldZone->max().z());

    // update map view
    GuiMapedit::window->view()->getZones()->set_active_zone(NULL);
    GuiMapedit::window->view()->updateOverlay();
    GuiMapedit::window->view()->draw();
}

// "make it so!"
void GuiZoneDialog::applyChanges()
{
    GObject *widget;
    
    // update name
    widget = gtk_builder_get_object (Ui, "entry_id");
    Zone->set_name (gtk_entry_get_text (GTK_ENTRY(widget)));
    
    // update type
    u_int32 type = 0;
    widget = gtk_builder_get_object (Ui, "cb_meta");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) type |= world::zone::TYPE_META; 
    widget = gtk_builder_get_object (Ui, "cb_render");
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget))) type |= world::zone::TYPE_RENDER; 
    Zone->set_type (type);

    // update map view
    GuiMapedit::window->view()->getZones()->set_active_zone(NULL);
    GuiMapedit::window->view()->updateOverlay();
    GuiMapedit::window->view()->draw();

    okButtonPressed (true);
}
