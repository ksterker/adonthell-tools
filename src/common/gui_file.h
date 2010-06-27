/*
   $Id: gui_file.h,v 1.1 2009/03/29 12:27:25 ksterker Exp $

   Copyright (C) 2002/2004 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file common/gui_file.h
 *
 * @author Kai Sterker
 * @brief Wrapper for the GtkFileSelection widget
 */

#include <string>
#include "gui_modal_dialog.h"

/**
 * A C++ interface to GTK's file selection dialog.
 */
class GuiFile : public GuiModalDialog
{
public:
    /**
     * Create a new file selection dialog.
     * @param parent the parent window for the dialog.
     * @param action type of file dialog.
     * @param file name of default file.
     * @param title the dialog title.
     */
    GuiFile (GtkWindow *parent, GtkFileChooserAction action, const std::string &file, const std::string &title);
    
    /**
     * Cleanup.
     */
    ~GuiFile ();

    /**
     * Show the dialog.
     * @return true if closed with okay, false otherwise.
     */
    bool run ();
    
    /**
     * Add a filter to limit the files that will be visible and
     * selectable by the user.
     * @param pattern only show files matching the pattern.
     * @param name name of the filter.
     */
    void add_filter (const std::string & pattern, const std::string & name);
    
    /**
     * Add shortcut to the given directory to the file chooser.
     * @param shortcut the shortcut to add.
     */
    void GuiFile::add_shortcut (const std::string & shortcut);
    
    /**
     * Return the users selection.
     * @param the users selection.
     */
    std::string getSelection () const          { return File; }

private:
    /// the file the user has selected
    std::string File;             
};
