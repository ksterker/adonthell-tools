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
 * @file mapedit/gui_script_selector.cc
 *
 * @author Kai Sterker
 * @brief assign python scripts and arguments.
 */

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <base/base.h>
#include <world/action.h>
#include <world/schedule.h>

#include "gui_script_selector.h"

// destroy schedule arguments
static void on_clear_args (gpointer data)
{
    PyObject *args = (PyObject*) data;
    Py_DECREF(args);
}

// user selects a different schedule
static void on_script_changed (GtkComboBox *cbox, gpointer user_data)
{
    gchar *scrpt_name = NULL;
    
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter (cbox, &iter))
    {
        GtkTreeModel *model = gtk_combo_box_get_model (cbox);
        gtk_tree_model_get (model, &iter, 0, &scrpt_name, -1);
    }
    
    GuiScriptSelector *selector = (GuiScriptSelector*) user_data;
    selector->script_selected (scrpt_name ? scrpt_name : "");
}

// user selects a different method
static void on_method_changed (GtkComboBox *cbox, gpointer user_data)
{
    gchar *met_name = NULL;
    
    GtkTreeIter iter;
    if (gtk_combo_box_get_active_iter (cbox, &iter))
    {
        GtkTreeModel *model = gtk_combo_box_get_model (cbox);
        gtk_tree_model_get (model, &iter, 0, &met_name, -1);
    }
    
    GuiScriptSelector *selector = (GuiScriptSelector*) user_data;
    selector->method_selected (met_name ? met_name : "");
}

// ctor
GuiScriptSelector::GuiScriptSelector (GtkComboBox *scrpt, GtkComboBox *met, GtkContainer *args)
{
    Script = scrpt;
    Method = met;
    Arguments = args;
    FilterMethod = "";
    CurScript = "";
    CurMethod = "";
    ArgOffset = 0;
    
    ScriptChangedHandler = g_signal_connect (G_OBJECT(scrpt), "changed", G_CALLBACK (on_script_changed), this);
    if (met != NULL)
    {
        MethodChangedHandler = g_signal_connect (G_OBJECT(met), "changed", G_CALLBACK (on_method_changed), this);
    }
}

// dtor
GuiScriptSelector::~GuiScriptSelector ()
{
    g_signal_handler_disconnect (G_OBJECT(Script), ScriptChangedHandler);
    if (Method != NULL)
    {
        g_signal_handler_disconnect (G_OBJECT(Method), MethodChangedHandler);
    }
}

// set place containing python scripts
void GuiScriptSelector::set_script_package (const std::string & dir)
{
    ScriptPkg = dir;
    
    std::string scandir = dir;
    for (unsigned long i = 0; i < scandir.length(); i++)
    {
        if (scandir[i] == '.') scandir[i] = '/';
    }

    GtkListStore *script_list = GTK_LIST_STORE (gtk_combo_box_get_model (Script));
    gtk_list_store_clear (script_list);

    scan_script_dir (base::Paths().game_data_dir () + scandir);
    scan_script_dir (base::Paths().user_data_dir () + scandir);    
}

// find all valid manager schedule scripts
void GuiScriptSelector::scan_script_dir (const std::string & script_dir)
{
    DIR *dir;
    GtkTreeIter iter;
    struct dirent *dirent;
    struct stat statbuf;
    
    // open directory
    if ((dir = opendir (script_dir.c_str ())) != NULL)
    {
        GtkListStore *script_list = GTK_LIST_STORE (gtk_combo_box_get_model (Script));
        printf ("*** info: scanning '%s' for python scripts.\n", script_dir.c_str());
        
        // read directory contents
        while ((dirent = readdir (dir)) != NULL)
        {
            // skip anything starting with .
            if (dirent->d_name[0] == '.') continue;
            
            string filepath = script_dir + dirent->d_name;             
            if (stat (filepath.c_str (), &statbuf) != -1)
            {
                // scripts are .py files
                if (S_ISREG (statbuf.st_mode) && filepath.compare (filepath.length() - 3, 3, ".py") == 0)
                {
                    // check if this is a valid script
                    std::string name (dirent->d_name, strlen(dirent->d_name) - 3);
                    if ("__init__" == name) continue;
                    
                    printf ("  - trying '%s' ... ", name.c_str());

                    // import module
                    PyObject *module = PyImport_ImportModule ((char *) (ScriptPkg + name).c_str ());
                    if (!module) 
                    {
                        printf ("import failed\n");
                        continue;
                    }
                    
                    // try to get class
                    PyObject *classobj = PyObject_GetAttrString (module, (char *) name.c_str ());
                    Py_DECREF (module);
                    if (!classobj)
                    {
                        printf ("class '%s' not found in script\n", name.c_str());
                        continue;
                    }
                    
                    // check class object for required method
                    bool hasMet = true;
                    if (FilterMethod.length() > 0)
                    {
                        hasMet = PyObject_HasAttrString (classobj, (char *) FilterMethod.c_str());
                    }

                    Py_DECREF (classobj);
                    if (!hasMet)
                    {
                        printf ("method '%s' not found in class\n", FilterMethod.c_str());
                        continue;
                    }
                    
                    // passed, so add it to list
                    gchar *script_name = g_strdup (name.c_str());
                    
                    // FIXME: check if script is already contained
                    
                    // get new row
                    gtk_list_store_append (script_list, &iter);
                    
                    // set our data
                    gtk_list_store_set (script_list, &iter, 0, script_name, -1);                        
                    
                    printf ("okay\n");
                }
            }
        }
        
        closedir (dir);
    }
}

// init the selector from a python script object
void GuiScriptSelector::init (const python::script *scrpt)
{
    if (scrpt != NULL)
    {
        // select script in drop down list
        select_entry (Script, scrpt->class_name());
        
        // set argument value(s)
        set_arguments (scrpt->get_args());
    }
    else
    {
        // clear selection
        gtk_combo_box_set_active (Script, -1);
        update_arg_table (NULL, 0);
    }
}

// init from method and arguments
void GuiScriptSelector::init (const python::method *met, PyObject *args)
{
    if (met != NULL)
    {
        // select script in drop down list
        select_entry (Script, met->script ());

        // select method in drop down list
        select_entry (Method, met->name ());
        
        // set argument value(s)
        set_arguments (args);
    }
    else
    {
        // clear selection
        gtk_combo_box_set_active (Script, -1);
        gtk_combo_box_set_active (Method, -1);
        update_arg_table (NULL, 0);
    }
}

// get argument list
PyObject *GuiScriptSelector::get_arguments () const
{
    PyObject *args = (PyObject*) g_object_get_data (G_OBJECT(Arguments), "num-args");
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
        printf ("*** info: collecting %li argument(s)\n", num_args);
        
        GList *children = gtk_container_get_children (Arguments);
        while (children != NULL)
        {
            GtkWidget *widget = GTK_WIDGET(children->data);
            const gchar* name = gtk_buildable_get_name (GTK_BUILDABLE(widget));
            if (strncmp ("entry_args", name, 10) == 0)
            {
                int index;
                if (sscanf(name, "entry_args_%d", &index) == 1)
                {
                    char *pend = NULL;
                    const gchar *val = gtk_entry_get_text (GTK_ENTRY(widget));
                    printf ("  - arg %i = %s\n", index, val);

                    // try cast to int first
                    PyObject *py_val = PyInt_FromString((char*) val, &pend, 0);
                    if (py_val == NULL || *pend != NULL)
                    {
                        // fallback to string on error
                        py_val = PyString_FromString(val);
                    }

                    PyTuple_SET_ITEM (args, index, py_val);
                }
            }
            
            children = g_list_next (children);
        }
    }
    
    return args;
}

// new script selected in list
void GuiScriptSelector::script_selected (const std::string & name)
{
    CurScript = name;
    if (CurScript == "")
    {
        return;
    }
    
    if (Method == NULL)
    {
        // we have a class and need to gather the arguments required for the constructor
        method_selected ("__init__");
    }
    else
    {
        // import module
        PyObject *module = PyImport_ImportModule ((char*)(ScriptPkg + CurScript).c_str());
        if (!module) return;
        
        // try to get class
        PyObject *classobj = PyObject_GetAttrString (module, (char *) CurScript.c_str());
        Py_DECREF (module);
        if (!classobj) return;
        
        // extract all methods of this class
        get_class_methods (classobj);
        
        // reset arguments
        update_arg_table (NULL, 0);
        
        // cleanup
        Py_DECREF (classobj);
    }
}

// new method selected in list
void GuiScriptSelector::method_selected (const std::string & name)
{
    CurMethod = name;
    if (CurMethod == "")
    {
        return;
    }
    
    // When a user selects the method, we need to figure out
    // if there are additional arguments required to call it 
    // and create the neccessary input fields on the fly. 
    
    // import module
    PyObject *module = PyImport_ImportModule ((char*)(ScriptPkg + CurScript).c_str());
    if (!module) return;
    
    // try to get class object of same name
    PyObject *classobj = PyObject_GetAttrString (module, (char *) CurScript.c_str());
    Py_DECREF (module);
    if (!classobj) return;
    
    // read method arguments
    get_method_arguments (classobj, name);

    // cleanup
    Py_DECREF (classobj);
}

// get the methods of a python class
void GuiScriptSelector::get_class_methods (PyObject *classobj)
{
    PyObject *globals = PyDict_New ();
    if (globals != NULL)
    {
        // need access to the inspect module ...
        PyObject *inspect = PyImport_ImportModule ("inspect");
        PyDict_SetItemString (globals, "inspect", inspect);
        // ... and the object whose constructor args we want to retrieve
        PyDict_SetItemString (globals, "x", classobj);
        
        std::string cmd = "meths = [a[0] for a in inspect.getmembers(x, inspect.ismethod)];";
        PyObject *result = python::run_string (cmd, Py_file_input, globals);
        if (result != NULL)
        {
            // get result from globals
            PyObject *meths = PyDict_GetItemString (globals, "meths");
            if (PyList_Check(meths))
            {
                GtkTreeIter iter;
                GtkListStore *method_list = GTK_LIST_STORE (gtk_combo_box_get_model (Method));
                gtk_list_store_clear (method_list);

                long len = PyList_GET_SIZE(meths);
                for (long i = 0; i < len; i++)
                {
                    // get method name
                    PyObject *met = PyList_GET_ITEM(meths, i);
                    gchar* met_name = PyString_AsString (met);
                    
                    // append the new row
                    gtk_list_store_append (method_list, &iter);
                    gtk_list_store_set (method_list, &iter, 0, met_name, -1);                        
                }                    
            }
            
            Py_DECREF(result);
            Py_XDECREF(meths);
        }
        
        Py_DECREF(globals);
        Py_XDECREF(inspect);
    }
}

// get the arguments of a python method
void GuiScriptSelector::get_method_arguments (PyObject *classobj, const std::string & method)
{
    PyObject *globals = PyDict_New ();
    if (globals != NULL)
    {
        // need access to the inspect module ...
        PyObject *inspect = PyImport_ImportModule ("inspect");
        PyDict_SetItemString (globals, "inspect", inspect);
        // ... and the object whose constructor args we want to retrieve
        PyDict_SetItemString (globals, "x", classobj);
        
        // evaluating "x.<method>.im_func.func_code.co_varnames" raises
        // a "RuntimeError: restricted attribute", but using the builtin
        // inspect module will work.
        std::string cmd = "args = inspect.getargspec(x." + method + ")[0];";
        PyObject *result = python::run_string (cmd, Py_file_input, globals);
        if (result != NULL)
        {
            // get result from globals
            PyObject *args = PyDict_GetItemString (globals, "args");
            if (PyList_Check(args))
            {
                // iterate over the list of constructor arguments
                // we can skip the first (self) and subsequent fixed arguments
                int offset = ArgOffset + 1; 
                long len = PyList_GET_SIZE(args);
                std::string arg_list[len-offset];
                for (long i = offset; i < len; i++)
                {
                    PyObject *arg = PyList_GET_ITEM(args, i);
                    const char* arg_name = PyString_AsString (arg);
                    arg_list[i-offset] = std::string(arg_name);
                }
                
                update_arg_table (arg_list, len-offset);
            }
            
            Py_DECREF(result);
            Py_XDECREF(args);
        }
        
        Py_DECREF(globals);
        Py_XDECREF(inspect);
    }
}

// create UI for editing arguments
void GuiScriptSelector::update_arg_table (std::string *arg_list, const long & len)
{
    int rows;
    // count remaining table rows --> we have to keep those    
    g_object_get(G_OBJECT(Arguments), "n-rows", &rows, NULL);
    
    // remove previous widgets
    GList *children = gtk_container_get_children (GTK_CONTAINER(Arguments));
    while (children != NULL)
    {
        GtkWidget *widget = GTK_WIDGET(children->data);
        const gchar* name = gtk_buildable_get_name (GTK_BUILDABLE (widget));
        if (strncmp ("entry_args", name, 10) == 0)
        {
            gtk_container_remove (GTK_CONTAINER(Arguments), widget);
        }
        else if (strncmp ("lbl_args", name, 8) == 0)
        {
            gtk_container_remove (GTK_CONTAINER(Arguments), widget);
            rows--;
        }
        children = g_list_next (children);
    }
    
    // resize table
    gtk_table_resize (GTK_TABLE(Arguments), len == 0 ? rows + 1 : rows + len, 2);
    
    // add new widgets
    if (len == 0)
    {
        GtkWidget* label = gtk_label_new ("No arguments required");
        gtk_buildable_set_name (GTK_BUILDABLE (label), "lbl_args");
        gtk_misc_set_alignment (GTK_MISC(label), 0.5f, 0.5f);
        gtk_widget_set_sensitive (label, false);
        gtk_table_attach_defaults (GTK_TABLE(Arguments), label, 0, 2, rows, rows + 1);
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
            gtk_buildable_set_name (GTK_BUILDABLE (label), tmp);
            gtk_misc_set_alignment (GTK_MISC(label), 1.0f, 0.5f);
            gtk_table_attach (GTK_TABLE(Arguments), label, 0, 1, i+rows, i+rows+1, fill, fill_expand, 0, 0);
            
            // add argument value entry
            GtkWidget *entry = gtk_entry_new ();
            sprintf (tmp, "entry_args_%i", i);
            gtk_buildable_set_name (GTK_BUILDABLE (entry), tmp);
            gtk_table_attach (GTK_TABLE(Arguments), entry, 1, 2, i+rows, i+rows+1, fill_expand, fill_expand, 0, 0);
        }
    }
    
    // store argument tuple
    PyObject *args = PyTuple_New (len);
    g_object_set_data_full (G_OBJECT(Arguments), "num-args", (gpointer) args, on_clear_args);
    
    // make everything visible
    gtk_widget_show_all (GTK_WIDGET(Arguments));
}

// set given arguments in UI
void GuiScriptSelector::set_arguments (PyObject *args)
{
    if (args != NULL)
    {
        GList *children = gtk_container_get_children (Arguments);
        while (children != NULL)
        {
            GtkWidget *widget = GTK_WIDGET(children->data);
            const gchar* name = gtk_buildable_get_name (GTK_BUILDABLE(widget));
            if (strncmp ("entry_args", name, 10) == 0)
            {
                int index;
                if (sscanf(name, "entry_args_%d", &index) == 1)
                {
                    PyObject *arg = PyTuple_GetItem (args, index + ArgOffset);
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

// set combobox selection 
void GuiScriptSelector::select_entry (GtkComboBox *cb, const std::string & entry)
{
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_combo_box_get_model (cb);
    
    if (gtk_tree_model_get_iter_first (model, &iter))
    {
        gchar *val;
        do
        {
            gtk_tree_model_get (model, &iter, 0, &val, -1);
            if (strcmp (entry.c_str(), val) == 0)
            {
                gtk_combo_box_set_active_iter (cb, &iter);
                break;
            }
        }
        while (gtk_tree_model_iter_next (model, &iter));
    }
    
    // not found? 
    if (gtk_combo_box_get_active (cb) < 0)
    {
        // smells fishy, but lets keep existing data
        gchar *val = g_strdup (entry.c_str());
        gtk_list_store_append (GTK_LIST_STORE (model), &iter);
        gtk_list_store_set (GTK_LIST_STORE (model), &iter, 0, val, -1);
        gtk_combo_box_set_active_iter (cb, &iter);
    }
}
