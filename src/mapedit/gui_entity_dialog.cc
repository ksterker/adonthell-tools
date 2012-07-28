/* 
 Copyright (C) 2009/2010 Kai Sterker <kaisterker@linuxgames.com>
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

#include <adonthell/world/character.h>

#include "common/util.h"
#include "common/uid.h"

#include "map_cmdline.h"
#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_entity_dialog.h"
#include "gui_entity_list.h"
#include "gui_script_selector.h"

// Ui definition
static char edit_entity_ui[] =
#include "entity-properties.glade.h"
;

#define SCENERY_PAGE    1
#define CHARACTER_PAGE  2
#define ITEM_PAGE       3

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

// center view on given object's location
static void on_center_button_pressed (GtkButton * button, gpointer user_data)
{
    MapEntity *entity = (MapEntity *) user_data;
    world::chunk_info *location = entity->getLocation();
    GuiMapedit::window->view()->gotoPosition(location->Min.x(), location->Min.y(), location->Min.z());
}

// edit object
static void on_edit_button_pressed (GtkButton * button, gpointer user_data)
{
    GError *error = NULL;
    MapEntity *entity = (MapEntity *) user_data;
    std::string path = MapCmdline::datadir + "/" + MapCmdline::project + "/" + entity->object()->modelfile();
    GdkScreen *screen = gtk_widget_get_screen (GTK_WIDGET(button));
    gchar *uri = g_filename_to_uri (util::get_absolute_path (path).c_str(), NULL, &error);

    if (uri != NULL)
    {
        if (gtk_show_uri (screen, uri, GDK_CURRENT_TIME, &error))
        {
            g_free (uri);
            return;
        }
    }

    g_message ("Opening model %s failed: %s", path.c_str(), error->message);
    g_error_free (error);
}

// the entity object state has changed
static void on_state_changed (GtkComboBox *cbox, gpointer user_data)
{
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter (cbox, &iter))
    {
        gchar *state_name;
        GtkTreeModel *model = gtk_combo_box_get_model (cbox);
        gtk_tree_model_get (model, &iter, 0, &state_name, -1);
        
        GuiEntityDialog *dlg = (GuiEntityDialog *) user_data;
        dlg->set_entity_state (state_name);
    }
}

// the entity object type has changed
static void on_type_changed (GtkToggleButton * button, gpointer user_data)
{
    if (gtk_toggle_button_get_active (button))
    {
        GuiEntityDialog *dlg = (GuiEntityDialog *) user_data;
        const gchar *id = gtk_buildable_get_name (GTK_BUILDABLE(button));
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

// callback for location changed
static void selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        // get location at selected row
        world::chunk_info *loc;
        gtk_tree_model_get (model, &iter, 1, &loc, -1);

        GuiEntityDialog *dlg = (GuiEntityDialog *) user_data;
        dlg->set_location (loc);
    }
}

// ctor
GuiEntityDialog::GuiEntityDialog (MapEntity *entity, const GuiEntityDialog::Mode & mode) 
    : GuiModalDialog (GTK_WINDOW(GuiMapedit::window->getWindow()))
{
    GError *err = NULL;
    GObject *widget;
    
    DlgMode = mode;
    Ui = gtk_builder_new();
    Entity = entity;
    
    // set defaults
    EntityType = 'A';
    EntityState = "";
    ObjType = world::OBJECT;
    Selector = NULL;
        
	if (!gtk_builder_add_from_string(Ui, edit_entity_ui, -1, &err)) 
    {
        g_message ("building entity dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    init_locations();
    
    // get reference to dialog window
    window = GTK_WIDGET (gtk_builder_get_object (Ui, "entity_properties"));
    switch (mode)
    {
        case UPDATE_PROPERTIES:
        {
            gtk_window_set_title (GTK_WINDOW (window), "Update Entity Properties");
            break;
        }
        case ADD_ENTITY_TO_MAP:
        {
            gtk_window_set_title (GTK_WINDOW (window), "Add new Entity to Map");
            break;
        }
        case DUPLICATE_NAMED_ENTITY:
        {
            gtk_window_set_title (GTK_WINDOW (window), "Copy Entity");
            break;
        }
    }
    
    // setup callbacks
    widget = gtk_builder_get_object (Ui, "btn_okay");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_ok_button_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_cancel");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_cancel_button_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_goto");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_center_button_pressed), Entity);
    widget = gtk_builder_get_object (Ui, "btn_edit_model");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_edit_button_pressed), Entity);
    
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

    widget = gtk_builder_get_object (Ui, "cb_state");
    g_signal_connect (widget, "changed", G_CALLBACK (on_state_changed), this);

    widget = gtk_builder_get_object (Ui, "location_list");
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
    g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(selected_event), this);

    // disable goto button if there is no location to go to
    widget = gtk_builder_get_object (Ui, "btn_goto");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), Entity->getLocation() != NULL);
    
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
    set_object_type (obj_type);
    switch (obj_type)
    {
        case world::OBJECT: 
        {
            world::chunk_info *loc = Entity->getLocation();
            if (loc != NULL) init_from_scenery (loc);
            break;
        }
        case world::CHARACTER: 
        {
            world::character *chr = dynamic_cast<world::character*>(entity->object());
            if (chr != NULL) init_from_character (chr);
            break;
        }
        case world::ITEM: 
        {
            break;
        }
        default: 
        {
            break;
        }
    }
    
    // in editing mode, change of object type is no longer allowed
    if (mode == UPDATE_PROPERTIES)
    {
        widget = gtk_builder_get_object (Ui, "type_scenery");
        gtk_widget_set_sensitive (GTK_WIDGET(widget), false);
        widget = gtk_builder_get_object (Ui, "type_character");
        gtk_widget_set_sensitive (GTK_WIDGET(widget), false);
        widget = gtk_builder_get_object (Ui, "type_item");
        gtk_widget_set_sensitive (GTK_WIDGET(widget), false);
    }
    
    // set entity type
    str = entity->get_entity_type ();
    switch (str[0])
    {
        case 'A':
        {
            widget = gtk_builder_get_object (Ui, "entity_anonymous");
            break;
        }
        case 'S':
        {
            widget = gtk_builder_get_object (Ui, "entity_shared");
            break;
        }
        case 'U':
        {
            widget = gtk_builder_get_object (Ui, "entity_unique");
            break;
        }
        default:
        {
            widget = NULL;
            break;
        }
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
    MapEntity *objToUpdate = Entity;
    const gchar* id = "";
    
    // get id, if neccessary
    if (EntityType == 'S' || EntityType == 'U')
    {
        GObject *widget = gtk_builder_get_object (Ui, "entity_id");
        id = gtk_combo_box_text_get_active_text (GTK_COMBO_BOX_TEXT (widget));
    }
    
    bool result; 
    
    if (DlgMode == UPDATE_PROPERTIES)
    {
        gchar* currentType = Entity->MapEntity::get_entity_type();
        
        // changed entity type?
        if (EntityType != currentType[0])
        {
            // type change from Anonymous to Shared/Unique
            if (currentType[0] == 'A')
            {
                objToUpdate = new MapEntity (Entity->entity());
                objToUpdate->loadMetaData();
                result = objToUpdate->update_entity (ObjType, EntityType, id);
                
                // try to replace currently highlighted object
                world::chunk_info *pos = Entity->getLocation();
                if (result && Entity->removeAtCurLocation())
                {
                    objToUpdate->addToLocation(pos->Min);
                }
            }
            
            // type changed from Shared/Unique to Anonymous
            if (EntityType == 'A')
            {
                // TODO: 1. Find or create anonymous instance
                // 2. Remove old instance from map and entity list
            }
        }
        else
        {
            // entity name has possibly changed
            objToUpdate->rename (id);
        }
        
        g_free(currentType);
    }
    else
    {
        result = objToUpdate->update_entity (ObjType, EntityType, id);
    }
    
    if (result)
    {
        // set entity state
        objToUpdate->object()->set_state (EntityState);
        
        if (ObjType == world::OBJECT)
        {
            set_scenery_data (Entity->getLocation());
        }
        // set character schedule, if necessary
        if (ObjType == world::CHARACTER)
        {
            set_character_data ((world::character*)(objToUpdate->object()));
        }
    }

    // update object hash, in case id changed
    std::string *hash = (std::string*)(&objToUpdate->object()->hash());
    u_int32 int_hash = uid::hash(objToUpdate->object()->modelfile() + id);
    std::string new_hash = uid::as_string(int_hash);

    if(DlgMode == ADD_ENTITY_TO_MAP || new_hash != *hash)
    {
        MapData *map = (MapData*)(&objToUpdate->object()->map());
        while (map->findDuplicateHash(new_hash))
        {
            new_hash = uid::as_string(++int_hash);
        }
        hash->replace(hash->begin(), hash->end(), new_hash);
    }

    if (objToUpdate != Entity)
    {
        // add new entity to entity list
        GuiMapedit::window->entityList()->addEntity (objToUpdate);
    }
    
    okButtonPressed (result);
}

// update the object type
void GuiEntityDialog::set_object_type (const world::placeable_type & type)
{
    ObjType = type;
    
    GObject *cb_anonymous = gtk_builder_get_object (Ui, "entity_anonymous");
    GObject *cb_shared = gtk_builder_get_object (Ui, "entity_shared");
    GObject *cb_unique = gtk_builder_get_object (Ui, "entity_unique");
    
    delete Selector;
    
    // depending on the placeable type, only a number of entity types make sense
    switch (type)
    {
        case world::OBJECT:
        {
            Selector = new GuiScriptSelector (
                GTK_COMBO_BOX(gtk_builder_get_object (Ui, "cb_scripts")), 
                GTK_COMBO_BOX(gtk_builder_get_object (Ui, "cb_methods")),
                GTK_CONTAINER(gtk_builder_get_object (Ui, "tbl_scripts")));
            Selector->set_argument_offset (2);
            Selector->set_script_filter ("");
            Selector->set_script_package (ACTION_DIR);
                        
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            
            // actions are tied to a location
            set_page_active (SCENERY_PAGE, Entity->getLocation() != NULL);
            set_page_active (CHARACTER_PAGE, false);
            break;
        }
        case world::CHARACTER:
        {
            Selector = new GuiScriptSelector (
                GTK_COMBO_BOX(gtk_builder_get_object (Ui, "cb_schedule")), NULL,
                GTK_CONTAINER(gtk_builder_get_object (Ui, "tbl_schedule")));
            Selector->set_argument_offset (1);
            Selector->set_script_filter ("run");
            Selector->set_script_package (SCHEDULE_DIR);
            
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            // since this is the only possibly choice --> chose it
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cb_unique), TRUE);            
            
            set_page_active (SCENERY_PAGE, false);
            set_page_active (CHARACTER_PAGE, true);
            break;
        }
        case world::ITEM:
        {
            Selector = NULL;
            
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            // check if we have a valid choice --> if not chose a sensible default
            if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cb_anonymous)))
            {
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cb_shared), TRUE);            
            }
            
            set_page_active (SCENERY_PAGE, false);
            set_page_active (CHARACTER_PAGE, false);
            break;
        }
        default:
        {
            Selector = NULL;
            break;
        }
    }    
}

// enable or disable a page of the dialog
void GuiEntityDialog::set_page_active (const int & page, const bool & active)
{
    GtkNotebook *nb = GTK_NOTEBOOK (gtk_builder_get_object (Ui, "notebook1"));
    GtkWidget *child = gtk_notebook_get_nth_page (nb, page);
    
    if (active)
    {
        gtk_widget_show_all (child);
    }
    else
    {
        gtk_widget_hide (child);
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

// set entity state
void GuiEntityDialog::set_entity_state (const std::string & state)
{
    EntityState = state;
    Entity->object()->set_state (state);
    
    // update image
    GdkPixbuf *img = Entity->get_icon (64);
    GObject *widget = gtk_builder_get_object (Ui, "img_preview");
    gtk_image_set_from_pixbuf (GTK_IMAGE (widget), img);
    g_object_unref (img);
}

// init values on scenery page
void GuiEntityDialog::init_from_scenery (world::chunk_info *location)
{
    if (location->has_action())
    {
        world::action *act = location->get_action();
        Selector->init (act->get_method(), act->get_args());
    }
    else
    {
        Selector->init (NULL, NULL);
    }
}

// init values on character page
void GuiEntityDialog::init_from_character (world::character *chr)
{
    world::schedule *sdl = chr->get_schedule();
    const python::script *scr = sdl->get_manager();
    Selector->init (scr);
}

// set scenery specific data
void GuiEntityDialog::set_scenery_data (world::chunk_info *location)
{
    if (location == NULL) return;
    
    // get script name (if any)
    std::string script = Selector->get_script_name();
    if ("" == script) return;
    
    // get method name (if any)
    std::string method = Selector->get_method_name();
    if ("" == method) return;
    
    // get method arguments
    PyObject *args = Selector->get_arguments();
    
    // create hash for saving action
    std::stringstream out (std::ios::out);
    out << script << method << location->Min;

    // set selected action
    world::action *act = location->set_action(uid::as_string(uid::hash(out.str())));
    act->init (script, method, args);
    Py_DECREF(args);
}

// set character specific data
void GuiEntityDialog::set_character_data (world::character *chr)
{
    // get schedule name (if any)
    std::string name = Selector->get_script_name();
    if ("" == name) return;
    
    // get schedule arguments
    PyObject *args = Selector->get_arguments();
    
    // set selected schedule
    world::schedule *sdl = chr->get_schedule();
    sdl->set_manager (name, args);
}

// set locations of the selected object
void GuiEntityDialog::init_locations ()
{
    GtkTreeIter iter;
    GObject *widget = gtk_builder_get_object (Ui, "location_list");
    
    GtkTreeModel *location_list = GTK_TREE_MODEL(gtk_tree_view_get_model (GTK_TREE_VIEW(widget)));
    gtk_list_store_clear (GTK_LIST_STORE(location_list));
    
    world::entity *ety = Entity->entity();
    if (ety != NULL)
    {
        MapData *map = (MapData*) &(Entity->object()->map());    

        const std::list<world::chunk_info*> locations = map->getEntityLocations (ety);
        for (std::list<world::chunk_info*>::const_iterator i = locations.begin(); i != locations.end(); i++)
        {
            std::stringstream out (std::ios::out);
            out << (*i)->Min;
            gchar *val = g_strdup (out.str().c_str());
            
            gtk_list_store_append (GTK_LIST_STORE (location_list), &iter);
            gtk_list_store_set (GTK_LIST_STORE (location_list), &iter, 0, val, 1, *i, -1);
            
            if (*i == Entity->getLocation())
            {
                // select it
                GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
                gtk_tree_selection_select_iter (selection, &iter);
                
                // and scroll it into view
                GtkTreePath *path = gtk_tree_model_get_path (location_list, &iter);
                gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(widget), path, NULL, TRUE, 0.5f, 0.0f);
                gtk_tree_path_free (path);
            }
        }
        
        gtk_tree_sortable_set_sort_func (GTK_TREE_SORTABLE(location_list), 0, sort_strings, NULL, NULL);
        gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(location_list), 0, GTK_SORT_ASCENDING);
    }
}

// update location of the entity being edited
void GuiEntityDialog::set_location (world::chunk_info *location)
{
    // update current location with selected values
    world::chunk_info *cur_loc = Entity->getLocation();
    if (cur_loc != NULL)
    {
        set_scenery_data (cur_loc);
    }
    
    // set new location
    Entity->setLocation (location);
    init_from_scenery (location);
}
