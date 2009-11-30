/* 
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
 * @file mapedit/gui_entity_dialog.cc
 *
 * @author Kai Sterker
 * @brief View and edit entity properties.
 */

#include <gtk/gtk.h>

#include "gui_mapedit.h"
#include "gui_entity_dialog.h"

// Ui definition
static char edit_entity_ui[] =
#include "entity-properties.glade.h"
;

static gint sort_strings (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer unused)
{
    GValue val_a = {0};
    GValue val_b = {0};
    
    gtk_tree_model_get_value (model, a, 0, &val_a);
    gtk_tree_model_get_value (model, b, 0, &val_b);
    
    return strcmp(g_value_get_string (&val_a), g_value_get_string (&val_b));
}

// close the dialog and keep all changes
static void on_ok_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiEntityDialog *dialog = (GuiEntityDialog *) user_data;
    
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

// the entity object type has changed
static void on_type_changed (GtkToggleButton * button, gpointer user_data)
{
    if (gtk_toggle_button_get_active (button))
    {
        GuiEntityDialog *dlg = (GuiEntityDialog *) user_data;
        const gchar *id = gtk_widget_get_name (GTK_WIDGET(button));
        if (strcmp (id, "type_scenery") == 0)
        {
            dlg->set_object_type (world::OBJECT);
        }
        else if (strcmp (id, "type_character") == 0)
        {
            dlg->set_object_type (world::CHARACTER);
        }
        else if (strcmp (id, "type_item") == 0)
        {
            dlg->set_object_type (world::ITEM);
        }
        else if (strcmp (id, "entity_anonymous") == 0)
        {
            dlg->set_entity_type ('A');
        }
        else if (strcmp (id, "entity_shared") == 0)
        {
            dlg->set_entity_type ('S');
        }
        else if (strcmp (id, "entity_unique") == 0)
        {
            dlg->set_entity_type ('U');
        }
    }
}

// ctor
GuiEntityDialog::GuiEntityDialog (MapEntity *entity, const GuiEntityDialog::Mode & mode) 
    : GuiModalDialog (GTK_WINDOW(GuiMapedit::window->getWindow()))
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    Entity = entity;
    
    // set defaults
    EntityType = 'A';
    ObjType = world::OBJECT;
    
	if (!gtk_builder_add_from_string(Ui, edit_entity_ui, -1, &err)) 
    {
        g_message ("building entity dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get reference to dialog window
    window = GTK_WIDGET (gtk_builder_get_object (Ui, "entity_properties"));
    switch (mode)
    {
        case READ_ONLY:
            gtk_window_set_title (GTK_WINDOW (window), "View Entity Properties");
            break;
        case ADD_ENTITY_TO_MAP:
            gtk_window_set_title (GTK_WINDOW (window), "Add new Entity to Map");
            break;
        case DUPLICATE_NAMED_ENTITY:
            gtk_window_set_title (GTK_WINDOW (window), "Chose unique id for Entity");
            break;
    }
    
    // setup callbacks
    widget = gtk_builder_get_object (Ui, "btn_okay");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_ok_button_pressed), this);
    gtk_widget_set_sensitive (GTK_WIDGET (widget), mode != READ_ONLY);
    widget = gtk_builder_get_object (Ui, "btn_cancel");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_cancel_button_pressed), this);
    
    widget = gtk_builder_get_object (Ui, "type_scenery");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "type_character");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "type_item");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);

    widget = gtk_builder_get_object (Ui, "entity_anonymous");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "entity_shared");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "entity_unique");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    
    // disable goto button (until we actually have a use for it)
    widget = gtk_builder_get_object (Ui, "btn_goto");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), FALSE);
    
    // set name
    gchar *str = entity->get_name();
    widget = gtk_builder_get_object (Ui, "lbl_name");
    gtk_label_set_text (GTK_LABEL (widget), str);
    g_free (str);
    
    // set id
    if (mode == DUPLICATE_NAMED_ENTITY)
    {
        str = entity->createNewId();
    }
    else
    {
        str = entity->get_id();
    }
    
    if (str != NULL)
    {
        widget = gtk_builder_get_object (Ui, "entity_id");
        
        if (mode == DUPLICATE_NAMED_ENTITY)
        {
            gtk_widget_grab_focus (GTK_WIDGET (widget));
        }
            
        GtkWidget *entry = gtk_bin_get_child (GTK_BIN(widget));
        gtk_entry_set_text (GTK_ENTRY (entry), str);
        g_free (str);
    }
    
    // set image
    GdkPixbuf *img = entity->get_icon (64);
    widget = gtk_builder_get_object (Ui, "img_preview");
    gtk_image_set_from_pixbuf (GTK_IMAGE (widget), img);
    g_object_unref (img);
    
    // set placeable states
    GtkTreeIter iter;
    widget = gtk_builder_get_object (Ui, "cb_state");
    
    GtkTreeModel *state_list = gtk_combo_box_get_model (GTK_COMBO_BOX(widget));
    gtk_list_store_clear (GTK_LIST_STORE (state_list));

    std::hash_set<std::string> states = entity->get_object_states();
    for (std::hash_set<std::string>::const_iterator i = states.begin(); i != states.end(); i++)
    {
        gchar *val = g_strdup (i->c_str());
        gtk_list_store_append (GTK_LIST_STORE (state_list), &iter);
        gtk_list_store_set (GTK_LIST_STORE (state_list), &iter, 0, val, -1);
        
        if (*i == entity->object()->state())
        {
            gtk_combo_box_set_active_iter (GTK_COMBO_BOX(widget), &iter);
        }
    }

    gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE(state_list), 0, sort_strings, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(state_list), 0, GTK_SORT_ASCENDING);
    
    // set placeable type
    world::placeable_type obj_type = entity->get_object_type();
    switch (obj_type)
    {
        case world::OBJECT:
            widget = gtk_builder_get_object (Ui, "type_scenery");
            break;
        case world::CHARACTER:
            widget = gtk_builder_get_object (Ui, "type_character");
            break;
        case world::ITEM:
            widget = gtk_builder_get_object (Ui, "type_item");
            break;
        default:
            widget = NULL;
            break;
    }
    if (widget != NULL)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
    }
    
    // set entity type
    str = entity->get_entity_type ();
    switch (str[0])
    {
        case 'A':
            widget = gtk_builder_get_object (Ui, "entity_anonymous");
            break;
        case 'S':
            widget = gtk_builder_get_object (Ui, "entity_shared");
            break;
        case 'U':
            widget = gtk_builder_get_object (Ui, "entity_unique");
            break;
        default:
            widget = NULL;
            break;
    }
    if (widget != NULL)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
    }
    g_free (str);
}

// dtor
GuiEntityDialog::~GuiEntityDialog()
{
    // cleanup
    g_object_unref (Ui);
}

// "make it so!"
void GuiEntityDialog::applyChanges()
{
    const gchar* id = "";
    // get id, if neccessary
    if (EntityType == 'S' || EntityType == 'U')
    {
        GObject *widget = gtk_builder_get_object (Ui, "entity_id");
        id = gtk_combo_box_get_active_text (GTK_COMBO_BOX (widget));
    }
    
    bool result = Entity->update_entity (ObjType, EntityType, id);
    okButtonPressed (result);
}

// update the object type
void GuiEntityDialog::set_object_type (const world::placeable_type & type)
{
    ObjType = type;
    
    GObject *cb_anonymous = gtk_builder_get_object (Ui, "entity_anonymous");
    GObject *cb_shared = gtk_builder_get_object (Ui, "entity_shared");
    GObject *cb_unique = gtk_builder_get_object (Ui, "entity_unique");
        
    // depending on the placeable type, only a number of entity types make sense
    switch (type)
    {
        case world::OBJECT:
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            break;
        case world::CHARACTER:
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            // since this is the only possibly choice --> chose it
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cb_unique), TRUE);            
            break;
        case world::ITEM:
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            // check if we have a valid choice --> if not chose a sensible default
            if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cb_anonymous)))
            {
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cb_shared), TRUE);            
            }
            break;
        default:
            break;
    }    
}

// update the entity type
void GuiEntityDialog::set_entity_type (const char & type)
{
    EntityType = type;

    // disable id field for anonymous entities ...
    GObject *widget = gtk_builder_get_object (Ui, "entity_id");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), type != 'A');
    // ... and grab focus for the rest
    if (type != 'A') gtk_widget_grab_focus (GTK_WIDGET (widget));
}
