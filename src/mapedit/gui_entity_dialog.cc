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

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <gtk/gtk.h>

#include <base/base.h>
#include <world/character.h>

#include "gui_mapedit.h"
#include "gui_entity_dialog.h"
#include "gui_entity_list.h"

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

// destroy schedule arguments
static void on_clear_args (gpointer data)
{
    PyObject *args = (PyObject*) data;
    Py_DECREF(args);
}

// create UI for editing schedule arguments
static void update_arg_table (GtkTable *arg_table, std::string *arg_list, const long & len)
{
    // remove previous widgets
    GList *children = gtk_container_get_children (GTK_CONTAINER(arg_table));
    while (children != NULL)
    {
        GtkWidget *widget = GTK_WIDGET(children->data);
        const gchar* name = gtk_widget_get_name (widget);
        if (strncmp ("entry_args", name, 10) == 0 ||
            strncmp ("lbl_args", name, 8) == 0)
        {
            gtk_container_remove (GTK_CONTAINER(arg_table), widget);
        }
        children = g_list_next (children);
    }
    
    // resize table
    gtk_table_resize (arg_table, len == 0 ? 2: len + 1, 2);
    
    // add new widgets
    if (len == 0)
    {
        GtkWidget* label = gtk_label_new ("No arguments required");
        gtk_widget_set_name (label, "lbl_args");
        gtk_misc_set_alignment (GTK_MISC(label), 0.5f, 0.5f);
        gtk_widget_set_sensitive (label, false);
        gtk_table_attach_defaults (arg_table, label, 0, 2, 1, 2);
    }
    else
    {
        char tmp[64];
        GtkAttachOptions fill = GTK_FILL;
        GtkAttachOptions fill_expand = (GtkAttachOptions) (GTK_FILL | GTK_EXPAND);
        
        for (int i = 0; i < len; i++)
        {
            // add argument name
            GtkWidget* label = gtk_label_new (arg_list[i].c_str());
            sprintf (tmp, "lbl_args_%i", i);
            gtk_widget_set_name (label, tmp);
            gtk_misc_set_alignment (GTK_MISC(label), 1.0f, 0.5f);
            gtk_table_attach (arg_table, label, 0, 1, i+1, i+2, fill, fill_expand, 0, 0);
            
            // add argument value entry
            GtkWidget *entry = gtk_entry_new ();
            sprintf (tmp, "entry_args_%i", i);
            gtk_widget_set_name (entry, tmp);
            gtk_table_attach (arg_table, entry, 1, 2, i+1, i+2, fill_expand, fill_expand, 0, 0);
        }
    }
    
    // store argument tuple
    PyObject *args = PyTuple_New (len);
    g_object_set_data_full (G_OBJECT(arg_table), "num-args", (gpointer) args, on_clear_args);
    
    // make everything visible
    gtk_widget_show_all (GTK_WIDGET(arg_table));
}

// user selects a different schedule
static void on_schedule_changed (GtkComboBox *cbox, gpointer user_data)
{
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter (cbox, &iter))
    {
        gchar *schedule_name;
        GtkTreeModel *model = gtk_combo_box_get_model (cbox);
        gtk_tree_model_get (model, &iter, 0, &schedule_name, -1);

        // When a user selects the script, we need to figure out
        // if there are additional arguments required for the 
        // constructor and create the neccessary input fields on the fly. 
        
        // import module
        PyObject *module = PyImport_ImportModule ((char*)(SCHEDULE_DIR + std::string (schedule_name)).c_str());
        if (!module) return;
        
        // try to get schedule class
        PyObject *classobj = PyObject_GetAttrString (module, schedule_name);
        Py_DECREF (module);
        if (!classobj) return;
        
        PyObject *globals = PyDict_New ();
        if (globals != NULL)
        {
            // need access to the inspect module ...
            PyObject *inspect = PyImport_ImportModule ("inspect");
            PyDict_SetItemString (globals, "inspect", inspect);
            // ... and the object whose constructor args we want to retrieve
            PyDict_SetItemString (globals, "x", classobj);
            
            // evaluating "x.__init__.im_func.func_code.co_varnames" raises
            // a "RuntimeError: restricted attribute", but using the builtin
            // inspect module will work.
            PyObject *result = python::run_string ("args = inspect.getargspec(x.__init__)[0];", Py_file_input, globals);
            if (result != NULL)
            {
                // get result from globals
                PyObject *args = PyDict_GetItemString (globals, "args");
                if (PyList_Check(args))
                {
                    // iterate over the list of constructor arguments
                    // we can skip the first (self) and second (schedule) 
                    long len = PyList_GET_SIZE(args);
                    std::string arg_list[len-2];
                    for (long i = 2; i < len; i++)
                    {
                        PyObject *arg = PyList_GET_ITEM(args, i);
                        const char* arg_name = PyString_AsString (arg);
                        arg_list[i-2] = std::string(arg_name);
                    }
                    
                    GtkTable *arg_table = GTK_TABLE(user_data);
                    update_arg_table (arg_table, arg_list, len-2);
                }
                
                Py_DECREF(result);
                Py_XDECREF(args);
            }
            
            Py_DECREF(globals);
            Py_XDECREF(inspect);
        }
        
        Py_DECREF(classobj);
    }
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
    
    DlgMode = mode;
    Ui = gtk_builder_new();
    Entity = entity;
    
    // set defaults
    EntityType = 'A';
    EntityState = "";
    ObjType = world::OBJECT;
    
	if (!gtk_builder_add_from_string(Ui, edit_entity_ui, -1, &err)) 
    {
        g_message ("building entity dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    setLocations();
    
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
    widget = gtk_builder_get_object (Ui, "cb_schedule");
    g_signal_connect (widget, "changed", G_CALLBACK (on_schedule_changed), 
                      gtk_builder_get_object (Ui, "tbl_schedule"));
    
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
    
    // set valid manager schedules
    scanMgrSchedules();
    
    // set placeable type
    world::placeable_type obj_type = entity->get_object_type();
    switch (obj_type)
    {
        case world::OBJECT: 
        {
            widget = gtk_builder_get_object (Ui, "type_scenery");
            set_page_active (SCENERY_PAGE, Entity->getLocation() != NULL);
            set_page_active (CHARACTER_PAGE, false);
            break;
        }
        case world::CHARACTER: 
        {
            widget = gtk_builder_get_object (Ui, "type_character");
            world::character *chr = dynamic_cast<world::character*>(entity->object());
            if (chr != NULL) init_from_character (chr);
            set_page_active (SCENERY_PAGE, false);
            break;
        }
        case world::ITEM: 
        {
            widget = gtk_builder_get_object (Ui, "type_item");
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
        id = gtk_combo_box_get_active_text (GTK_COMBO_BOX (widget));
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
        
        // set character schedule, if neccessary
        if (ObjType == world::CHARACTER)
        {
            set_character_data ((world::character*) objToUpdate->object());
        }
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
        
    // depending on the placeable type, only a number of entity types make sense
    switch (type)
    {
        case world::OBJECT:
        {
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
        gtk_widget_hide_all (child);
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
    
    // update image
    std::string prevState = Entity->object()->state();
    Entity->object()->set_state (state);
    
    GdkPixbuf *img = Entity->get_icon (64);
    GObject *widget = gtk_builder_get_object (Ui, "img_preview");
    gtk_image_set_from_pixbuf (GTK_IMAGE (widget), img);
    g_object_unref (img);
    
    Entity->object()->set_state (prevState);
}

// find all valid manager schedule scripts
void GuiEntityDialog::scanMgrSchedules ()
{
    std::string scheduledir = base::Paths.user_data_dir () + "/schedules/char";

    DIR *dir;
    GtkTreeIter iter;
    struct dirent *dirent;
    struct stat statbuf;
    
    GObject *widget = gtk_builder_get_object (Ui, "cb_schedule");    
    GtkListStore *schedule_list = GTK_LIST_STORE (gtk_combo_box_get_model (GTK_COMBO_BOX(widget)));
    gtk_list_store_clear (schedule_list);
    
    // open directory
    if ((dir = opendir (scheduledir.c_str ())) != NULL)
    {
        // read directory contents
        while ((dirent = readdir (dir)) != NULL)
        {
            // skip anything starting with .
            if (dirent->d_name[0] == '.') continue;
            
            string filepath = scheduledir + "/";
            filepath += dirent->d_name; 
            
            if (stat (filepath.c_str (), &statbuf) != -1)
            {
                // schedules are .py files
                if (S_ISREG (statbuf.st_mode) && filepath.compare (filepath.length() - 3, 3, ".py") == 0)
                {
                    // check if this is a valid manager schedule
                    std::string name (dirent->d_name, strlen(dirent->d_name) - 3);
                    
                    // import module
                    PyObject *module = PyImport_ImportModule ((char *) (SCHEDULE_DIR + name).c_str ());
                    if (!module) continue;
                    
                    // try to get schedule class
                    PyObject *classobj = PyObject_GetAttrString (module, (char *) name.c_str ());
                    Py_DECREF (module);
                    if (!classobj) continue;
                    
                    // check class object for run method
                    bool hasRunMet = PyObject_HasAttrString (classobj, "run");
                    Py_DECREF (classobj);
                    if (!hasRunMet) continue;
                    
                    // passed, so add it to list
                    gchar *mgr_schedule = g_strdup (name.c_str());
                    
                    // get new row
                    gtk_list_store_append (schedule_list, &iter);
                    
                    // set our data
                    gtk_list_store_set (schedule_list, &iter, 0, mgr_schedule, -1);                        
                }
            }
        }
        
        closedir (dir);
    }
}

// init values on character page
void GuiEntityDialog::init_from_character (world::character *chr)
{
    world::schedule *sdl = chr->get_schedule();
    const python::script *scr = sdl->get_manager();
    if (scr != NULL)
    {
        GtkTreeIter iter;
        GtkComboBox *widget = GTK_COMBO_BOX(gtk_builder_get_object (Ui, "cb_schedule"));    
        GtkTreeModel *model = gtk_combo_box_get_model (widget);

        // select script in drop down list
        std::string name = scr->class_name();
        if (gtk_tree_model_get_iter_first (model, &iter))
        {
            gchar *val;
            do
            {
                gtk_tree_model_get (model, &iter, 0, &val, -1);
                if (strcmp (name.c_str(), val) == 0)
                {
                    gtk_combo_box_set_active_iter (widget, &iter);
                    break;
                }
            }
            while (gtk_tree_model_iter_next (model, &iter));
        }

        // not found? 
        if (gtk_combo_box_get_active (widget) < 0)
        {
            // smells fishy, but lets keep existing data
            gchar *val = g_strdup (name.c_str());
            gtk_list_store_append (GTK_LIST_STORE (model), &iter);
            gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, val, -1);
            gtk_combo_box_set_active_iter (widget, &iter);
        }
        
        // set argument value(s)
        PyObject *args = scr->get_args();
        if (args != NULL)
        {
            GtkContainer *arg_table = GTK_CONTAINER (gtk_builder_get_object (Ui, "tbl_schedule"));
            GList *children = gtk_container_get_children (arg_table);
            while (children != NULL)
            {
                GtkWidget *widget = GTK_WIDGET(children->data);
                const gchar* name = gtk_widget_get_name (widget);
                if (strncmp ("entry_args", name, 10) == 0)
                {
                    int index;
                    if (sscanf(name, "entry_args_%d", &index) == 1)
                    {
                        PyObject *arg = PyTuple_GetItem (args, index+1); // first arg is the schedule itself
                        if (arg != NULL)
                        {
                            PyObject *val = PyObject_Str (arg);
                            gtk_entry_set_text (GTK_ENTRY(widget), g_strdup(PyString_AsString(val)));
                            Py_DECREF(val);
                        }
                    }
                }
                
                children = g_list_next (children);
            }   
        }
    }
}

// set character specific data
void GuiEntityDialog::set_character_data (world::character *chr)
{
    // get schedule name (if any)
    GtkComboBox *widget = GTK_COMBO_BOX(gtk_builder_get_object (Ui, "cb_schedule"));
    const gchar *value = gtk_combo_box_get_active_text (GTK_COMBO_BOX (widget));
    if (value == NULL) return;
    
    std::string name (value);

    // get schedule arguments
    GtkContainer *arg_table = GTK_CONTAINER (gtk_builder_get_object (Ui, "tbl_schedule"));
    PyObject *args = (PyObject*) g_object_get_data (G_OBJECT(arg_table), "num-args");
    if (args != NULL)
    {
        Py_INCREF (args);
    }
    else
    {
        args = PyTuple_New (0);
    }
    
    long num_args = PyTuple_GET_SIZE(args);
    if (num_args > 0)
    {
        GList *children = gtk_container_get_children (arg_table);
        while (children != NULL)
        {
            GtkWidget *widget = GTK_WIDGET(children->data);
            const gchar* name = gtk_widget_get_name (widget);
            if (strncmp ("entry_args", name, 10) == 0)
            {
                int index;
                if (sscanf(name, "entry_args_%d", &index) == 1)
                {
                    char *pend = NULL;
                    PyObject *py_val = NULL;
                    const gchar *val = gtk_entry_get_text (GTK_ENTRY(widget));
                    // try cast to int first
                    py_val = PyInt_FromString((char*) val, &pend, 0);
                    if (py_val == NULL || *pend != NULL)
                    {
                        // fallback to string on error
                        py_val = PyString_FromString(val);
                    }
                    PyTuple_SetItem (args, index, py_val);
                }
            }
            
            children = g_list_next (children);
        }
    }
    
    // set selected schedule
    world::schedule *sdl = chr->get_schedule();
    sdl->set_manager (name, args);
}

// set locations of the selected object
void GuiEntityDialog::setLocations ()
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
