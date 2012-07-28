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
 * @file mapedit/gui_script_selector.h
 *
 * @author Kai Sterker
 * @brief assign python scripts and arguments.
 */

#ifndef GUI_SCRIPT_SELECTOR_H
#define GUI_SCRIPT_SELECTOR_H

#include <gtk/gtk.h>
#include <adonthell/python/script.h>

/**
 * Helper class to select from a list of python scripts,
 * an (optional) list of methods from the selected scripts
 * and finally fill in the the arguments required either
 * for the scripts constructor or the selected method.
 */
class GuiScriptSelector
{
public:
    /**
     * Create a script selector instance.
     * @param scrpt a combobox used to present the python scripts/classes.
     * @param met a combobox used to present the methods of the selected scripts (or NULL)
     * @param args a table to edit the arguments for method or constructor call.
     */
    GuiScriptSelector (GtkComboBox *scrpt, GtkComboBox *met, GtkContainer *args);
    
    /**
     * Destroys a script selector instance.
     */
    ~GuiScriptSelector ();

    /**
     * @name Customize selector behaviour.
     */
    //@{
    /**
     * Set the Python package that contains the available scripts.
     * Should be relative to the respective game data directory.
     * @param dir a python package.
     */
    void set_script_package (const std::string & dir);

    /**
     * Set the number of fixed arguments. Most scripts used by the
     * Adonthell engine have some arguments that are supplied by the
     * engine itself, followed by an optional number of custom arguments.
     * This method allows to exclude former, so only the custom arguments
     * are presented to and editable by the user.
     * @param number of arguments to exclude from editing.
     */
    void set_argument_offset (const int & offset) { ArgOffset = offset; }
    
    /**
     * Some scripts require a certain method to be present. Setting this
     * method name as filter assures that only those scripts show up in
     * the list that implement this method. Set to "" to disable the filter.
     * @param filter a method name to filter scripts by.
     */
    void set_script_filter (const std::string & filter) { FilterMethod = filter; }    
    //@}
    
    /**
     * @name Setting initial state.
     */
    //@{
    /**
     * Initialize selector that only displays a script selection.
     * Arguments are set from the scripts constructor.
     * @param scrpt the script being preselected.
     */
    void init (const python::script *scrpt);
    
    /**
     * Initialize selector that displays both script and method
     * selection. Arguments are those used to call the given method.
     * @param met the script and method to preselect.
     * @param args the method arguments.
     */
    void init (const python::method *met, PyObject *args);
    //@}
    
    /**
     * @name Result extraction
     */
    //@{
    /**
     * Get name of the selected script.
     * @return Script the user has selected.
     */
    std::string get_script_name () const { return CurScript; }
    
    /**
     * Get name of the selected script.
     * @return Script the user has selected.
     */
    std::string get_method_name () const { return CurMethod; }
    
    /**
     * Get tuple containing the optional arguments.
     * @return Arguments the user has specified.
     */
    PyObject *get_arguments () const;
    //@}

    /**
     * @name Callbacks.
     */
    //@{
    /**
     * Called when the user selected a new item in
     * the list of scripts.
     * @param name name of the selected script.
     */
    void script_selected (const std::string & name);

    /**
     * Called when the user selected a new item in
     * the list of methods.
     * @param name name of the selected method.
     */
    void method_selected (const std::string & name);    
    //@}
    
private:
    void scan_script_dir (const std::string & script_dir);
    void update_arg_table (std::string *arg_list, const long & len);
    void set_arguments (PyObject *args);
    void get_class_methods (PyObject *classobj);
    void get_method_arguments (PyObject *classobj, const std::string & method);
    void select_entry (GtkComboBox *cb, const std::string & entry);

    /// a list of scripts contained in the package directory
    GtkComboBox *Script;
    /// a list of methods contained in the script 
    GtkComboBox *Method;
    /// a GtkTable to edit the method arguments
    GtkContainer *Arguments;
    /// scripts to include must implement this method
    std::string FilterMethod;
    /// the package from which to load scripts 
    std::string ScriptPkg;
    /// the currently selected script
    std::string CurScript;
    /// the currently selected method
    std::string CurMethod;
    /// number of fixed arguments past to the method
    int ArgOffset;
    /// event handler id for script combobox changes
    long ScriptChangedHandler;
    /// event handler id for method combobox changes
    long MethodChangedHandler;
};

#endif
