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
#include <world/schedule.h>

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

    widget = gtk_builder_get_object (Ui, "cb_state");
    g_signal_connect (widget, "changed", G_CALLBACK (on_state_changed), this);
    
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
            widget = gtk_builder_get_object (Ui, "type_scenery");
            set_page_active (1, false);
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
            set_page_active (1, false);
            break;
        case world::CHARACTER:
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            // since this is the only possibly choice --> chose it
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cb_unique), TRUE);            
            set_page_active (1, true);
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
            set_page_active (1, false);
            break;
        default:
            break;
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
    Entity->object()->set_state (state);
    
    // update image
    GdkPixbuf *img = Entity->get_icon (64);
    GObject *widget = gtk_builder_get_object (Ui, "img_preview");
    gtk_image_set_from_pixbuf (GTK_IMAGE (widget), img);
    g_object_unref (img);
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
        python::script scr;
        world::schedule sdl;
        PyObject *args = PyTuple_New (1);
        PyTuple_SetItem (args, 0, python::pass_instance(&sdl));
        
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
                    std::string name (dirent->d_name, strlen(dirent->d_name) - 3);
                    
                    /*
                    // TODO: the code below works as long as the manager schedules do not
                    // expect additional args in their constructor. To be truly useful, we
                    // would have to load the module, then check for a class by the same 
                    // name, then figure out if that has a run method without actually
                    // instanciating it:
                     
                    // import module
                    PyObject *module = PyImport_ImportModule ((char *) SCHEDULE_DIR + name.c_str ());
                    if (!module) continue;
                    
                    // get schedule class
                    PyObject *classobj = PyObject_GetAttrString (module, (char *) name.c_str ());
                    Py_DECREF (module);
                    if (!classobj) continue;

                    // instanciate without calling constructor
                    PyObject *instance = PyInstance_NewRaw (classobj, NULL);
                    Py_DECREF (classobj);
                    if (!instance) continue;

                    // check for run method
                    bool hasRunMet = PyObject_HasAttrString (Instance, "run");
                    Py_DECREF (instance);
                    if (!hasRunMet) continue;
                    
                    // When a user selects the script, we would then need to figure out
                    // if there are arguments required for the constructor and create the
                    // neccessary input fields on the fly. In Python we can do this:
                    
                    >>> class x:
                    ...    def __init__(self, x, y):
                    ...       pass
                    ...
                    >>> import inspect
                    >>> inspect.getargspec(x.__init__)
                    (['self', 'x', 'y'], None, None, None)
                    
                    Much simpler, this roughly boils down to 
                    
                    >>> print x.__init__.im_func.func_code.co_varnames
                    ('self', 'x', 'y')
                    >>> print x.__init__.im_func.func_code.co_argcount       
                    3

                    // Can we do it in C as well?
                    */
                    
                    // now check if this is a valid manager schedule
                    if (!scr.reload_instance (SCHEDULE_DIR + name, name, args)) continue;
                    
                    //  --> needs a run method
                    if (!scr.has_attribute ("run")) continue;
                    
                    // passed, so add it to list
                    gchar *mgr_schedule = g_strdup (name.c_str());
                    
                    // get new row
                    gtk_list_store_append (schedule_list, &iter);
                    
                    // set our data
                    gtk_list_store_set (schedule_list, &iter, 0, mgr_schedule, -1);                        
                }
            }
        }
        
        // cleanup
        Py_DECREF (args);
        closedir (dir);
    }
}
