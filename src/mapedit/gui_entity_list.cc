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


#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <gtk/gtk.h>
#include <base/base.h>
#include <world/object.h>

#include "common/util.h"
#include "common/uid.h"

#include "map_cmdline.h"
#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_entity_list.h"
#include "gui_entity_dialog.h"
#include "gui_filter_dialog.h"

enum
{
    NAME_COLUMN,
    TYPE_COLUMN,
    ICON_COLUMN,
    COLOR_COLUMN,
    TOOLTIP_COLUMN,
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
static GtkTreeModelIface parent_iface = { { 0 }, };

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

// retrieve an object from our parent's data storage
static MapEntity *entity_list_get_object (EntityList *self, GtkTreeIter *iter)
{
	GValue value = { 0, };
	MapEntity *obj = NULL;
    
	// validate our parameters 
	g_return_val_if_fail (IS_ENTITY_LIST (self), NULL);
	g_return_val_if_fail (iter != NULL, NULL);
    
	// retrieve the object using our parent's interface, take our own reference to it
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
        G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_STRING
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
            gchar *name = obj->get_name ();
            gchar *id = obj->get_id ();
            if (id != NULL)
            {
                gchar *tmp = g_strconcat (name, "\n", id, NULL);
                g_free (name);
                g_free (id);
                name = tmp;
            }
            g_value_set_string (value, name);
            g_free (name);
            break;
        }   
		case TYPE_COLUMN:
        {
            gchar *type = obj->get_entity_type ();
			g_value_set_string (value, type);
            g_free (type);
			break;
        }   
		case ICON_COLUMN:
        {
			g_value_set_object (value, obj->get_icon ());
			break;
        }
        case COLOR_COLUMN:
        {
            const gchar *color = obj->isOnMap () ? (obj->getRefCount() ? "#FFFFFF" : "#80C8FE") : "#FEB380";
			g_value_set_string (value, color);
			break;
        }
        case TOOLTIP_COLUMN:
        {
            std::string path = obj->object()->modelfile();
            gchar* dir = g_path_get_dirname (path.c_str());
            gchar* set = g_path_get_basename (dir);

            const std::string &comment = obj->get_comment();
            if (comment.length() > 0)
            {
                gchar *tmp = g_strconcat (set, "\n", comment.c_str(), NULL);
                g_free (set);
                set = tmp;
            }

            g_value_set_string (value, set);
            g_free(set);
            g_free(dir);
            break;
        }
		default:
        {
			g_assert_not_reached ();
        }
	}
}

// callback for selection changes
static void selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *filterModel;
    GtkTreeIter filterIter;
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &filterModel, &filterIter))
    {
        GuiMapview *map_view = GuiMapedit::window->view();

        // get actual model
        model = gtk_tree_model_filter_get_model(GTK_TREE_MODEL_FILTER(filterModel));

        // get object at selected row
        gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER(filterModel), &iter, &filterIter);
        MapEntity *obj = (MapEntity*) entity_list_get_object (ENTITY_LIST (model), &iter);

        // check if object needs to be added to map
        if (!obj->isOnMap ())
        {
            // allow user to add entity to map
            GuiEntityDialog dlg (obj, GuiEntityDialog::ADD_ENTITY_TO_MAP);
            if (!dlg.run())
            {
                // user canceled and object has not been added to map,
                map_view->releaseObject();
                return;
            }
            
            // notify tree view of the change
            GtkTreePath *path = gtk_tree_model_get_path (filterModel, &filterIter);
            gtk_tree_model_row_changed (filterModel, path, &filterIter);
            gtk_tree_path_free (path);
        }
        
        // object has been added to map (or already had been there in the first place),
        // so make it the editing object
        map_view->selectObj(obj);

        // update entity list, if filtered by connectors
        if (GuiFilterDialog::getActiveFilter() == GuiFilterDialog::BY_CONNECTOR)
        {
            GuiEntityList *list = (GuiEntityList*) user_data;
            list->filterChanged();
        }
    }
}

// callback for adding new zones
static void on_refresh_entities (GtkButton * button, gpointer user_data)
{
    GuiEntityList *list = (GuiEntityList*) user_data;
    list->refresh();
}

// callback for adding new zones
static void on_filter_entities (GtkButton * button, gpointer user_data)
{
    GuiFilterDialog *dlg = new GuiFilterDialog (GuiFilterDialog::getFilterModel());
    dlg->run();
}

// check if given model should be filtered from entity list
static gboolean is_entity_visible (GtkTreeModel *model, GtkTreeIter *iter, gpointer user_data)
{
    MapEntity *obj = (MapEntity*) entity_list_get_object (ENTITY_LIST (model), iter);
    return !GuiFilterDialog::isEntityFiltered (obj);
}

// sort model list by model path and file name
static gint sort_by_path (GtkTreeModel *model, GtkTreeIter *a, GtkTreeIter *b, gpointer userdata)
{
    // get object at selected row
    MapEntity *obj_a = (MapEntity*) entity_list_get_object (ENTITY_LIST (model), a);
    MapEntity *obj_b = (MapEntity*) entity_list_get_object (ENTITY_LIST (model), b);

    // compare the two
    return strcmp (obj_a->object()->modelfile().c_str(), obj_b->object()->modelfile().c_str());
}

// ctor
GuiEntityList::GuiEntityList ()
{
    Panel = gtk_vbox_new (FALSE, 0);

    // the view
    TreeView = (GtkTreeView*) gtk_tree_view_new ();
    gtk_tree_view_set_grid_lines (TreeView, GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    GtkWidget *scrollWnd = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW(scrollWnd), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_container_add (GTK_CONTAINER(scrollWnd), GTK_WIDGET(TreeView));
    
    // the controls
    GtkWidget *btnRefresh = gtk_button_new ();
    gtk_button_set_image (GTK_BUTTON(btnRefresh), gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_BUTTON));
    gtk_widget_set_tooltip_text (GTK_WIDGET(btnRefresh), "Reload list of available entities from file system.");
    g_signal_connect (G_OBJECT(btnRefresh), "clicked", G_CALLBACK(on_refresh_entities), this);
    
    GtkWidget *btnFilter = gtk_button_new ();
    gtk_button_set_image (GTK_BUTTON(btnFilter), gtk_image_new_from_stock (GTK_STOCK_FIND, GTK_ICON_SIZE_BUTTON));
    gtk_widget_set_tooltip_text (GTK_WIDGET(btnFilter), "Filter list of available entities.");
    g_signal_connect (G_OBJECT(btnFilter), "clicked", G_CALLBACK(on_filter_entities), this);

    GtkWidget *btnPnl = gtk_hbutton_box_new();
    gtk_button_box_set_layout (GTK_BUTTON_BOX(btnPnl), GTK_BUTTONBOX_END);
    gtk_box_pack_start (GTK_BOX(btnPnl), btnFilter, FALSE, FALSE, 4);
    gtk_box_pack_start (GTK_BOX(btnPnl), btnRefresh, FALSE, FALSE, 4);
    
    gtk_box_pack_start (GTK_BOX(Panel), scrollWnd, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(Panel), btnPnl, FALSE, TRUE, 0);
    gtk_widget_show_all (Panel);
    
    // create the columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Name", renderer, "text", NAME_COLUMN, "background", COLOR_COLUMN, NULL);
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Type", renderer, "text", TYPE_COLUMN, "background", COLOR_COLUMN, NULL);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Icon", renderer, "pixbuf", ICON_COLUMN, NULL);
    
    // update column attributes
    GtkTreeViewColumn *col = gtk_tree_view_get_column (TreeView, NAME_COLUMN);
    gtk_tree_view_column_set_expand(col, true);

    // tooltips
    gtk_tree_view_set_tooltip_column(TreeView, TOOLTIP_COLUMN);

    // selection listener
    GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);
    SelectionChanged = g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(selected_event), this);
    
    // create the (empty) model
    GtkListStore *model = (GtkListStore *) g_object_new (TYPE_ENTITY_LIST, NULL);
    GtkTreeModel *filterModel = gtk_tree_model_filter_new (GTK_TREE_MODEL (model), NULL);

    // set filter function
    gtk_tree_model_filter_set_visible_func (GTK_TREE_MODEL_FILTER (filterModel), is_entity_visible, NULL, NULL);

    // set tree model
    gtk_tree_view_set_model (TreeView, filterModel);

    // set custom sorting by path name
    gtk_tree_sortable_set_default_sort_func(GTK_TREE_SORTABLE (model), sort_by_path, NULL, NULL);
    gtk_tree_sortable_set_sort_column_id(GTK_TREE_SORTABLE (model), GTK_TREE_SORTABLE_DEFAULT_SORT_COLUMN_ID, GTK_SORT_ASCENDING);
}

// return mapedit wrapper around given entity
MapEntity *GuiEntityList::findEntity (const world::entity *etyToFind) const
{
    GtkTreeIter iter;
    MapEntity *ety;
    
    // iterate over all entities in model
    GtkTreeModelFilter *filterModel = GTK_TREE_MODEL_FILTER (gtk_tree_view_get_model (TreeView));
    GtkTreeModel *model = gtk_tree_model_filter_get_model(filterModel);

    if (gtk_tree_model_get_iter_first (model, &iter))
    {
        do
        {
            ety = entity_list_get_object (ENTITY_LIST(model), &iter);
            
            // found the right entity?
            if (ety->entity() == etyToFind)
            {
                return ety;
            }
        }
        while (gtk_tree_model_iter_next (model, &iter));
    }
    
    return NULL;
}

// add given entity
void GuiEntityList::addEntity (MapEntity *ety)
{
    GtkTreeIter iter;
    
    // get model
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER (gtk_tree_view_get_model (TreeView));
    GtkListStore *model = GTK_LIST_STORE (gtk_tree_model_filter_get_model (filter));
    
    // get new row
    gtk_list_store_append (model, &iter);
    
    // set our data
    gtk_list_store_set (model, &iter, 0, ety, -1);    
    
    // select it and scroll it into view
    GtkTreePath *child_path = gtk_tree_model_get_path (GTK_TREE_MODEL (model), &iter);
    GtkTreePath *path = gtk_tree_model_filter_convert_child_path_to_path (filter, child_path);

    if (path != NULL)
    {
        GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);
        g_signal_handler_block (G_OBJECT(selection), SelectionChanged);
        gtk_tree_selection_select_path (selection, path);
        g_signal_handler_unblock (G_OBJECT(selection), SelectionChanged);

        gtk_tree_view_scroll_to_cell (TreeView, path, NULL, TRUE, 0.5f, 0.0f);
        gtk_tree_path_free (path);
    }

    gtk_tree_path_free (child_path);

    // update filter
    filterChanged();
}

// select given entity
bool GuiEntityList::setSelected (MapEntity *etyToSelect, const bool & select)
{
    GtkTreeIter filterIter;
    GtkTreeIter iter;
    MapEntity *ety;
    
    // iterate over all entities in model
    GtkTreeModel *filterModel = GTK_TREE_MODEL (gtk_tree_view_get_model (TreeView));
    GtkTreeModel *model = gtk_tree_model_filter_get_model(GTK_TREE_MODEL_FILTER (filterModel));

    if (gtk_tree_model_get_iter_first (filterModel, &filterIter))
    {
        do
        {
            gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (filterModel), &iter, &filterIter);
            ety = entity_list_get_object (ENTITY_LIST(model), &iter);
            
            // found the right entity?
            if (ety == etyToSelect)
            {
                GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);
                if (select)
                {
                    // scroll it into view
                    GtkTreePath *path = gtk_tree_model_get_path (filterModel, &filterIter);
                    gtk_tree_view_scroll_to_cell (TreeView, path, NULL, TRUE, 0.5f, 0.0f);
                    gtk_tree_path_free (path);

                    // make sure selected signal fires even if row already selected
                    if (gtk_tree_selection_iter_is_selected(selection, &filterIter))
                    {
                        gtk_tree_selection_unselect_iter (selection, &filterIter);
                    }

                    // and select it
                    gtk_tree_selection_select_iter (selection, &filterIter);
                }
                else
                {
                    // clear selection
                    gtk_tree_selection_unselect_iter (selection, &filterIter);                    
                }
                
                return true;
            }
        }
        while (gtk_tree_model_iter_next (filterModel, &filterIter));
    }
    
    return false;
}

// pick previous object in entity list for drawing
MapEntity *GuiEntityList::getPrev()
{
    GtkTreeIter filterIter;
    GtkTreeIter iter;

    MapEntity *ety = NULL;

    GtkTreeModel *filterModel = GTK_TREE_MODEL (gtk_tree_view_get_model (TreeView));
    GtkTreeModel *model = gtk_tree_model_filter_get_model(GTK_TREE_MODEL_FILTER (filterModel));

    // get selected entity
    GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);

    // there's something selected
    if (gtk_tree_selection_get_selected (selection, &filterModel, &filterIter))
    {
        GtkTreePath *path = gtk_tree_model_get_path (filterModel, &filterIter);
        if (gtk_tree_path_prev (path))
        {
            gtk_tree_model_get_iter (filterModel, &filterIter, path);
            gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (filterModel), &iter, &filterIter);
            ety = entity_list_get_object (ENTITY_LIST(model), &iter);
        }
        gtk_tree_path_free (path);
    }

    // otherwise select last in list
    if (ety == NULL)
    {
        int num_rows = gtk_tree_model_iter_n_children (filterModel, NULL);
        if (num_rows > 0)
        {
            char str[16];
            sprintf(str, "%i", num_rows - 1);

            GtkTreePath *path = gtk_tree_path_new_from_string (str);
            if (gtk_tree_model_get_iter (filterModel, &filterIter, path))
            {
                gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (filterModel), &iter, &filterIter);
                ety = entity_list_get_object (ENTITY_LIST(model), &iter);
            }
            gtk_tree_path_free (path);
        }
    }

    if (ety != NULL)
    {
        // select it in list, but not for drawing
        g_signal_handler_block (G_OBJECT(selection), SelectionChanged);
        gtk_tree_selection_select_iter (selection, &filterIter);
        g_signal_handler_unblock (G_OBJECT(selection), SelectionChanged);

        // and scroll it into view
        GtkTreePath *path = gtk_tree_model_get_path (filterModel, &filterIter);
        gtk_tree_view_scroll_to_cell (TreeView, path, NULL, TRUE, 0.5f, 0.0f);
        gtk_tree_path_free (path);
    }

    return ety;
}

// pick next object in entity list for drawing
MapEntity *GuiEntityList::getNext()
{
    GtkTreeIter filterIter;
    GtkTreeIter iter;

    MapEntity *ety = NULL;

    GtkTreeModel *filterModel = GTK_TREE_MODEL (gtk_tree_view_get_model (TreeView));
    GtkTreeModel *model = gtk_tree_model_filter_get_model(GTK_TREE_MODEL_FILTER (filterModel));

    // get selected entity
    GtkTreeSelection *selection = gtk_tree_view_get_selection (TreeView);

    // there's something selected and another row following
    if (gtk_tree_selection_get_selected (selection, &filterModel, &filterIter) &&
        gtk_tree_model_iter_next (filterModel, &filterIter))
    {
        gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (filterModel), &iter, &filterIter);
        ety = entity_list_get_object (ENTITY_LIST(model), &iter);
    }
    // otherwise select first in list
    else if (gtk_tree_model_get_iter_first (filterModel, &filterIter))
    {
        gtk_tree_model_filter_convert_iter_to_child_iter (GTK_TREE_MODEL_FILTER (filterModel), &iter, &filterIter);
        ety = entity_list_get_object (ENTITY_LIST(model), &iter);
    }

    if (ety != NULL)
    {
        // select it in list, but not for drawing
        g_signal_handler_block (G_OBJECT(selection), SelectionChanged);
        gtk_tree_selection_select_iter (selection, &filterIter);
        g_signal_handler_unblock (G_OBJECT(selection), SelectionChanged);

        // and scroll it into view
        GtkTreePath *path = gtk_tree_model_get_path (filterModel, &filterIter);
        gtk_tree_view_scroll_to_cell (TreeView, path, NULL, TRUE, 0.5f, 0.0f);
        gtk_tree_path_free (path);
    }

    return ety;
}

// set map being displayed
void GuiEntityList::setMap (MapData * map)
{
    if (Map == map) return;
    
    Map = map;
    GtkTreeIter iter;
    
    // get model
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER (gtk_tree_view_get_model (TreeView));
    GtkListStore *model = GTK_LIST_STORE (gtk_tree_model_filter_get_model (filter));
    
    // avoid tree updates while adding rows
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) NULL);

    // cleanup previously set data
    gtk_list_store_clear (model);
        
    // try to get model directory used by models of map
    MapCmdline::modeldir = map->getModelDirectory();

    // fill model
    for (MapData::entity_iter e = map->firstEntity(); e != map->lastEntity(); e++)
    {
        // create meta data object
        MapEntity *obj = new MapEntity (*e, map->getEntityCount (*e));
        
        // update tags of new entity
        obj->loadMetaData();

        // get new row
        gtk_list_store_append (model, &iter);
        
        // set our data
        gtk_list_store_set (model, &iter, 0, obj, -1);
    }
    
    // set the model again 
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) filter);
}

void GuiEntityList::setDataDir (const std::string & datadir)
{
    DataDir = datadir;
    
    // get model
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER (gtk_tree_view_get_model (TreeView));
    GtkListStore *model = GTK_LIST_STORE (gtk_tree_model_filter_get_model (filter));
    
    // avoid tree updates while adding rows
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) NULL);

    // add models contained under directory
    scanDir (datadir, model);
    
    // set the model again 
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) filter);
}

// recursively scan given directory for models
void GuiEntityList::scanDir (const std::string & datadir, GtkListStore *model)
{
    DIR *dir;
    GtkTreeIter iter;
    struct dirent *dirent;
    struct stat statbuf;
    
    // open directory
    if ((dir = opendir (datadir.c_str ())) != NULL)
    {
        // read directory contents
        while ((dirent = readdir (dir)) != NULL)
        {
            // skip anything starting with .
            if (dirent->d_name[0] == '.') continue;

            string filepath = datadir + "/";
            filepath += dirent->d_name; 
            
            if (stat (filepath.c_str (), &statbuf) != -1)
            {
                // recurse
                if (S_ISDIR (statbuf.st_mode)) scanDir (filepath, model);
                
                // models are .xml or .amdl files
                if (S_ISREG (statbuf.st_mode) && (
                        filepath.compare (filepath.length() - 4, 4, ".xml") == 0 ||
                        filepath.compare (filepath.length() - 4, 4, "amdl") == 0))
                {
                    // check if this file is already part of the map
                    if (isPresentOnMap (MK_UNIX_PATH(filepath))) continue;
                    
                    // not present on map, so add it to the list
                    
                    // try to create a relative sprite path
                    std::string model_path = util::get_relative_path (filepath, MapCmdline::modeldir + "/");
                    if (g_path_is_absolute (model_path.c_str()))
                    {
                        // FIXME: display error in status bar
                        printf ("*** warning: cannot create model path relative to data directory!\n");
                    }
                    
                    // note: we load it as an object, as we do not yet
                    // know which type it will have later.

                    // the objects created here are not yet part of the map, so the hash
                    // is preliminary. It may be changed for named entities and will be
                    // checked for uniqueness when placing the object on the map.
                    world::object *obj = new world::object(*Map, uid::as_string(uid::hash(model_path)));
                    if (obj->load_model (model_path))
                    {
                        // set default state
                        obj->set_state ("");
                        
                        // create meta data object
                        MapEntity *ety = new MapEntity (obj);
                        
                        // update tags of new entity
                        ety->loadMetaData();

                        // get new row
                        gtk_list_store_append (model, &iter);
                        
                        // set our data
                        gtk_list_store_set (model, &iter, 0, ety, -1);                        
                    }
                    else
                    {
                        printf ("*** warning: cannot load model '%s'!\n", model_path.c_str());
                    }
                }
            }
        }

        closedir (dir);
    }
}

// rebuild the entity list
void GuiEntityList::refresh()
{
    if (Map == NULL) return;
    
    GtkTreeIter iter;

    // make sure the connector templates are up-to-date
    MdlConnectorManager::reload(base::Paths().user_data_dir());

    // get model
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER (gtk_tree_view_get_model (TreeView));
    GtkListStore *model = GTK_LIST_STORE (gtk_tree_model_filter_get_model (filter));
    
    // avoid tree updates while adding rows
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) NULL);
    
    // clear all models not present on map
    bool valid = gtk_tree_model_get_iter_first (GTK_TREE_MODEL(model), &iter);
    while (valid)
    {
        MapEntity *ety = entity_list_get_object (ENTITY_LIST(model), &iter);
        
        if (ety->getRefCount() == 0)
        {
            // entity not yet on map --> reload all of it
            valid = gtk_list_store_remove (model, &iter);
            delete ety;
            continue;
        }
        else
        {
            // otherwise at least update meta data
            ety->loadMetaData();
        }

        valid = gtk_tree_model_iter_next (GTK_TREE_MODEL(model), &iter);
    }
    
    // add models contained under directory
    scanDir (DataDir, model);
    
    // set the model again 
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) filter);
}

// check if object with given name is already placed on map
bool GuiEntityList::isPresentOnMap (const std::string & filename) const
{
    if (Map == NULL) return false;

    // convert windows to unix directory separators
    std::string unix_filename = filename;
    for (unsigned long i = 0; i < unix_filename.length(); i++)
    {
        if (unix_filename[i] == '\\') unix_filename[i] = '/';
    }

    for (MapData::entity_iter e = Map->firstEntity(); e != Map->lastEntity(); e++)
    {
        world::placeable *obj = (*e)->get_object();
        std::string objname = obj->modelfile();

        if (objname.size() <= filename.size())
        {
            // convert windows to unix directory separators
            for (unsigned long i = 0; i < objname.length(); i++)
            {
                if (objname[i] == '\\') objname[i] = '/';
            }
            
            // files should be equal if they are equal except for a prefix
            if (unix_filename.compare (unix_filename.size() - objname.size(), objname.size(), objname) == 0)
            {
                return true;
            }
        }
    }
    
    return false;
}

// notify the entity list that it needs to refilter
void GuiEntityList::filterChanged ()
{
    GtkTreeModelFilter *filter = GTK_TREE_MODEL_FILTER (gtk_tree_view_get_model (TreeView));
    gtk_tree_model_filter_refilter (filter);
}
