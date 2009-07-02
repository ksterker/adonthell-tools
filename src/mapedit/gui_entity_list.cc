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
 * @file mapedit/gui_entity_list.cc
 *
 * @author Kai Sterker
 * @brief Display entities used on the map.
 */

#include <gtk/gtk.h>

#include "mapedit/gui_entity_list.h"

enum
{
    NAME_COLUMN,
    TYPE_COLUMN,
    ICON_COLUMN,
    NUM_COLUMNS
};

static void entity_list_tree_model_iface_init (GtkTreeModelIface *iface);
static int entity_list_get_n_columns (GtkTreeModel *self);
static GType entity_list_get_column_type (GtkTreeModel *self, int column);
static void entity_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value);

// EntityList inherits from GtkListStore, and implements the GtkTreeStore interface
G_DEFINE_TYPE_EXTENDED (EntityList, entity_list, GTK_TYPE_LIST_STORE, 0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, entity_list_tree_model_iface_init));

// our parent's model iface 
static GtkTreeModelIface parent_iface = { 0, };

// this method is called once to set up the class 
static void entity_list_class_init (EntityListClass *klass)
{
}

// this method is called once to set up the interface 
static void entity_list_tree_model_iface_init (GtkTreeModelIface *iface)
{
	// this is where we override the interface methods 
	// first make a copy of our parent's interface to call later 
	parent_iface = *iface;
    
	// now put in our own overriding methods 
	iface->get_n_columns = entity_list_get_n_columns;
	iface->get_column_type = entity_list_get_column_type;
	iface->get_value = entity_list_get_value;
}

// this method is called every time an instance of the class is created 
static void entity_list_init (EntityList *self)
{
	// initialise the underlying storage for the GtkListStore 
	GType types[] = { G_TYPE_POINTER };
    
	gtk_list_store_set_column_types (GTK_LIST_STORE (self), 1, types);
}

// retreive an object from our parent's data storage
static MapEntity *entity_list_get_object (EntityList *self, GtkTreeIter *iter)
{
	GValue value = { 0, };
	MapEntity *obj = NULL;
    
	// validate our parameters 
	g_return_val_if_fail (IS_ENTITY_LIST (self), NULL);
	g_return_val_if_fail (iter != NULL, NULL);
    
	// retreive the object using our parent's interface, take our own reference to it 
	parent_iface.get_value (GTK_TREE_MODEL (self), iter, 0, &value);
	obj = (MapEntity*) g_value_get_pointer (&value);
    
    // cleanup
	g_value_unset (&value);
    
	return obj;
}

// this method returns the number of columns in our tree model 
static int entity_list_get_n_columns (GtkTreeModel *self)
{
	return NUM_COLUMNS;
}

// this method returns the type of each column in our tree model 
static GType entity_list_get_column_type (GtkTreeModel *self, int column)
{
	static GType types[] = {
        G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF
	};
    
	// validate our parameters 
	g_return_val_if_fail (IS_ENTITY_LIST (self), G_TYPE_INVALID);
	g_return_val_if_fail (column >= 0 && column < NUM_COLUMNS, G_TYPE_INVALID);
    
	return types[column];
}

// this method retrieves the value for a particular column 
static void entity_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value)
{
	// validate our parameters 
	g_return_if_fail (IS_ENTITY_LIST (self));
	g_return_if_fail (iter != NULL);
	g_return_if_fail (column >= 0 && column < NUM_COLUMNS);
	g_return_if_fail (value != NULL);
    
	// get the object from our parent's storage
	MapEntity *obj = (MapEntity*) entity_list_get_object (ENTITY_LIST (self), iter);
    
	// initialise our GValue to the required type
	g_value_init (value, entity_list_get_column_type (GTK_TREE_MODEL (self), column));
    
	switch (column)
	{
        case NAME_COLUMN:
        {
            gchar *name = obj->get_name_and_id ();
            g_value_set_string (value, name);
            g_free (name);
            break;
        }   
		case TYPE_COLUMN:
        {
            gchar *type = obj->get_type_name ();
			g_value_set_string (value, type);
            g_free (type);
			break;
        }   
		case ICON_COLUMN:
        {
			g_value_set_object (value, obj->get_icon ());
			break;
        }   
		default:
        {
			g_assert_not_reached ();
        }
	}
}

// ctor
GuiEntityList::GuiEntityList ()
{
    // the view
    TreeView = (GtkTreeView*) gtk_tree_view_new ();
    gtk_tree_view_set_grid_lines (TreeView, GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    
    // create the columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Name", renderer, "text", NAME_COLUMN, NULL);
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Type", renderer, "text", TYPE_COLUMN, NULL);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Icon", renderer, "pixbuf", ICON_COLUMN, NULL);

    // create the (empty) model
    GtkListStore *model = (GtkListStore *) g_object_new (TYPE_ENTITY_LIST, NULL);
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) model);
}

// set map being displayed
void GuiEntityList::setMap (MapData * map)
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
    for (MapData::entity_iter e = map->firstEntity(); e != map->lastEntity(); e++)
    {
        // create meta data object
        MapEntity *obj = new MapEntity (*e);
        
        // get new row
        gtk_list_store_append (model, &iter);
        
        // set our data
        gtk_list_store_set (model, &iter, 0, obj, -1);
    }
    
    // set the model again 
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) model);
}
