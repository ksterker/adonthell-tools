/*
   $Id: gui_code.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $ 

   Copyright (C) 2002 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Dlgedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dlgedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**
 * @file gui_code.h
 *
 * @author Kai Sterker
 * @brief The Custom Code dialog.
 */

#ifndef GUI_CODE_H
#define GUI_CODE_H

#include "gui_edit.h"
#include "dlg_module_entry.h"

/**
 * Provides entries for additional import lines, for code to be inserted 
 * into constructor and destructor and for defining custom methods.
 */
class GuiCode
{
public:
    /**
     * Create empty Python Code Dialog.
     */
    GuiCode ();

    /**
     * Cleanup.
     */
    ~GuiCode ();
    
    /**
     * Global pointer to the custom code dialog
     */
    static GuiCode *dialog;
    
    /**
     * Show the dialog and display the contents of the given module.
     * @param entry the custom code of a certain DlgModule to be edited.
     * @param name name of the dialogue whose code is being edited.
     */
    void display (DlgModuleEntry *entry, const std::string &name);
    
    /**
     * Store the user's entries once the OK button has been pressed.
     */
    void applyChanges ();
    
private:
    DlgModuleEntry *entry;  // The module being edited
    GtkWidget *window;      // The dialog window
    GuiEdit *imports;       // entry for additional import statements
    GuiEdit *ctor;          // entry for code inserted into constructor
    GuiEdit *dtor;          // entry for code inserted into destructor
    GuiEdit *methods;       // entry for custom methods
};

#endif // GUI_CODE_H
