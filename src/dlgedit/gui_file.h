/*
   $Id: gui_file.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file gui_file.h
 *
 * @author Kai Sterker
 * @brief Wrapper for the GtkFileSelection widget
 */

#include <string>
#include "gui_modal_dialog.h"

/**
 * A C++ interfact to GTK's file selection dialog.
 */
class GuiFile : public GuiModalDialog
{
public:
    GuiFile (int type, const std::string &file, const std::string &title);
    ~GuiFile ();

    std::string getSelection ()           { return file_; }
    void setSelection (std::string file)  { file_ = file; }

private:
    std::string file_;              // the file the user has selected
};


/**
 * Callback to intercept pressing of fileselection's okay button
 */
void on_ok_button_pressed (GtkButton *button, gpointer user_data);
