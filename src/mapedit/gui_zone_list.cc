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
 * @file mapedit/gui_zone_list.cc
 *
 * @author Kai Sterker
 * @brief Display zones used on the map.
 */

#include <gtk/gtk.h>

#include <world/zone.h>

#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_zone_list.h"
#include "gui_zone_dialog.h"

enum
{
    NAME_COLUMN,
    TYPE_COLUMN,
    NUM_COLUMNS
};

static void zone_list_tree_model_iface_init (GtkTreeModelIface *iface);
static int zone_list_get_n_columns (GtkTreeModel *self);
static GType zone_list_get_column_type (GtkTreeModel *self, int column);
static void zone_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value);

// ZoneList inherits from GtkListStore, and implements the GtkTreeStore interface
G_DEFINE_TYPE_EXTENDED (ZoneList, zone_list, GTK_TYPE_LIST_STORE, 0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, zone_list_tree_model_iface_init));

// our parent's model iface 
static GtkTreeModelIface parent_iface = { 0, };

// this method is called once to set up the class 
static void zone_list_class_init (ZoneListClass *klass)
{
}

// this method is called once to set up the interface 
static void zone_list_tree_model_iface_init (GtkTreeModelIface *iface)
{
	// this is where we override the interface methods 
	// first make a copy of our parent's interface to call later 
	parent_iface = *iface;
    
	// now put in our own overriding methods 
	iface->get_n_columns = zone_list_get_n_columns;
	iface->get_column_type = zone_list_get_column_type;
	iface->get_value = zone_list_get_value;
}

// this method is called every time an instance of the class is created 
static void zone_list_init (ZoneList *self)
{
	// initialise the underlying storage for the GtkListStore 
	GType types[] = { G_TYPE_POINTER };
    
	gtk_list_store_set_column_types (GTK_LIST_STORE (self), 1, types);
}

// retreive an object from our parent's data storage
static world::zone *zone_list_get_object (ZoneList *self, GtkTreeIter *iter)
{
	GValue value = { 0, };
    world::zone *z = NULL;
    
	// validate our parameters 
	g_return_val_if_fail (IS_ZONE_LIST (self), NULL);
	g_return_val_if_fail (iter != NULL, NULL);
    
	// retreive the object using our parent's interface, take our own reference to it 
	parent_iface.get_value (GTK_TREE_MODEL (self), iter, 0, &value);
	z = (world::zone*) g_value_get_pointer (&value);
    
    // cleanup
	g_value_unset (&value);
    
	return z;
}

// this method returns the number of columns in our tree model 
static int zone_list_get_n_columns (GtkTreeModel *self)
{
	return NUM_COLUMNS;
}

// this method returns the type of each column in our tree model 
static GType zone_list_get_column_type (GtkTreeModel *self, int column)
{
	static GType types[] = {
        G_TYPE_STRING, G_TYPE_STRING
	};
    
	// validate our parameters 
	g_return_val_if_fail (IS_ZONE_LIST (self), G_TYPE_INVALID);
	g_return_val_if_fail (column >= 0 && column < NUM_COLUMNS, G_TYPE_INVALID);
    
	return types[column];
}

// this method retrieves the value for a particular column 
static void zone_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value)
{
	// validate our parameters 
	g_return_if_fail (IS_ZONE_LIST (self));
	g_return_if_fail (iter != NULL);
	g_return_if_fail (column >= 0 && column < NUM_COLUMNS);
	g_return_if_fail (value != NULL);
    
	// get the object from our parent's storage
    world::zone *z = (world::zone*) zone_list_get_object (ZONE_LIST (self), iter);
    
	// initialise our GValue to the required type
	g_value_init (value, zone_list_get_column_type (GTK_TREE_MODEL (self), column));
    
	switch (column)
	{
        case NAME_COLUMN:
        {
            gchar *name = g_strdup(z->name ().c_str());
            g_value_set_string (value, name);
            g_free (name);
            break;
        }   
		case TYPE_COLUMN:
        {
            gchar *type = g_strdup("");
			g_value_set_string (value, type);
            g_free (type);
			break;
        }   
		default:
        {
			g_assert_not_reached ();
        }
	}
}

// callback for adding new zones
static void on_add_zone (GtkButton * button, gpointer user_data)
{
    GuiZoneList *list = (GuiZoneList*) user_data;
    list->addZone();
}

// callback for deleting zones
static void on_remove_zone (GtkButton * button, gpointer user_data)
{
    GuiZoneList *list = (GuiZoneList*) user_data;
    list->removeZone();
}

// callback for editing zones
static void on_edit_zone (GtkTreeView *tree, GtkTreePath *path, GtkTreeViewColumn *column, gpointer user_data)
{
    GuiZoneList *list = (GuiZoneList*) user_data;
    list->editZone();
}

// callback for selection changes
static void selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        // get object at selected row
        world::zone *z = (world::zone*) zone_list_get_object (ZONE_LIST (model), &iter);

        // TODO: focus on zone in mapview
    }
}

// ctor
GuiZoneList::GuiZoneList ()
{
    Panel = gtk_vbox_new (FALSE, 0);
    
    // the tree view
    TreeView = (GtkTreeView*) gtk_tree_view_new ();
    gtk_tree_view_set_grid_lines (TreeView, GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);    
    GtkWidget *scrollWnd = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrollWnd), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_container_add (GTK_CONTAINER(scrollWnd), GTK_WIDGET(TreeView));
    g_signal_connect (G_OBJECT(TreeView), "row-activated", G_CALLBACK(on_edit_zone), this);
    
    // the controls
    GtkWidget *btnAdd = gtk_button_new ();
    gtk_button_set_image (GTK_BUTTON(btnAdd), gtk_image_new_from_stock (GTK_STOCK_ADD, GTK_ICON_SIZE_BUTTON));
    gtk_widget_set_tooltip_text (GTK_WIDGET(btnAdd), "Add new Zone to map");
    g_signal_connect (G_OBJECT(btnAdd), "clicked", G_CALLBACK(on_add_zone), this);

    GtkWidget *btnRemove = gtk_button_new ();
    gtk_button_set_image (GTK_BUTTON(btnRemove), gtk_image_new_from_stock (GTK_STOCK_REMOVE, GTK_ICON_SIZE_BUTTON));
    gtk_widget_set_tooltip_text (GTK_WIDGET(btnRemove), "Remove selected Zone from map");
    g_signal_connect (G_OBJECT(btnRemove), "clicked", G_CALLBACK(on_remove_zone), this);

    GtkWidget *btnPnl = gtk_hbutton_box_new();
    gtk_button_box_set_layout (GTK_BUTTON_BOX(btnPnl), GTK_BUTTONBOX_END);
    gtk_box_pack_start (GTK_BOX(btnPnl), btnAdd, FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX(btnPnl), btnRemove, FALSE, FALSE, 4);
    
    gtk_box_pack_start (GTK_BOX(Panel), scrollWnd, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(Panel), btnPnl, FALSE, TRUE, 0);
    gtk_widget_show_all (Panel);
    
    // create the columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Name", renderer, "text", NAME_COLUMN, NULL);
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Type", renderer, "text", TYPE_COLUMN, NULL);

    // selection listener
    GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);
    g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(selected_event), this);
    
    // create the (empty) model
    GtkListStore *model = (GtkListStore *) g_object_new (TYPE_ZONE_LIST, NULL);
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) model);
}

// create new zone
void GuiZoneList::addZone()
{    
    GtkAllocation allocation;
    gtk_widget_get_allocation (GuiMapedit::window->view()->drawingArea(), &allocation);

    // add new tile sized zone in the center of the map
    world::vector3<s_int32> min (Map->x() + allocation.width / 2, Map->y() + allocation.height / 2, Map->z());
    world::vector3<s_int32> max = min + world::vector3<s_int32> (48, 36, 8);
    world::zone *z = new world::zone(world::zone::TYPE_META, "", min, max);

    // add zone to map
    if (Map->add_zone(z))
    {
        GuiZoneDialog dlg (z, Map);
        if (dlg.run())
        {
            // add zone to list
            GtkTreeIter iter;
            GtkListStore *model = GTK_LIST_STORE (gtk_tree_view_get_model (TreeView));
            gtk_list_store_append (model, &iter);
            gtk_list_store_set (model, &iter, 0, z, -1);
            
            return;
        }

        // action canceled
        Map->remove_zone(z);

        // update map view
        GuiMapedit::window->view()->updateOverlay();
        GuiMapedit::window->view()->draw();
    }
    else
    {
        fprintf (stderr, "Zone '%s' already present on map!\n", z->name().c_str());
    }

    // cleanup
    delete z;
}

// delete selected zone
void GuiZoneList::removeZone()
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        // get object at selected row
        world::zone *z = (world::zone*) zone_list_get_object (ZONE_LIST (model), &iter);
        
        // remove zone from map
        Map->remove_zone (z);
        
        // remove zone from list
        gtk_list_store_remove  (GTK_LIST_STORE(model), &iter);
        
        // update map view
        GuiMapedit::window->view()->updateOverlay();
        GuiMapedit::window->view()->draw();

        // cleanup
        delete z;
    }        
}

// edit selected zone
void GuiZoneList::editZone()
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        // get object at selected row
        world::zone *z = (world::zone*) zone_list_get_object (ZONE_LIST (model), &iter);
        
        // bring up properties dialog
        GuiZoneDialog dlg (z, Map);
        dlg.run ();
    }    
}

// set map being displayed
void GuiZoneList::setMap (MapData * map)
{
    if (Map == map) return;
    
    Map = map;
    GtkTreeIter iter;
    
    // get model
    GtkListStore *model = GTK_LIST_STORE (gtk_tree_view_get_model (TreeView));
    
    // avoid tree updates while adding rows
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) NULL);

    // cleanup previously set data
    gtk_list_store_clear (model);
    
    // fill model
    for (MapData::zone_iter z = map->firstZone(); z != map->lastZone(); z++)
    {
        // get new row
        gtk_list_store_append (model, &iter);
        
        // set our data
        gtk_list_store_set (model, &iter, 0, *z, -1);
    }
    
    // set the model again 
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) model);
}
