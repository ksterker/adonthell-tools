/*
   $Id: gui_edit.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $ 

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
 * @file gui_edit.h
 *
 * @author Kai Sterker
 * @brief Wrapper around the GtkText widget.
 */

#include <string>
#include <gtk/gtk.h>
#include <gdk/gdk.h>

/**
 * Provides a simple C++ interface to GtkText. The widget is initialised
 * to use a font with fixed width, so it should only be used to edit code.
 */
class GuiEdit
{
public:
    /**
     * Create a new GtkText widget.
     * @param container Container to put the editor widget into.
     */
    GuiEdit (GtkWidget *container);
    ~GuiEdit ();

    /**
     * Get a pointer to the GtkText widget.
     * @return a pointer to the editor widget.
     */
    GtkWidget *widget ()            { return view; }

    /**
     * Set the text of the widget
     * @param text to display in the entry
     */
    void setText (const std::string &text);

    /**
     * Retrieve the text of the widget
     * @return text contained in the widget.
     */
    std::string getText ();

private:
    GtkTextBuffer *entry;		// The actual text buffer
    GtkWidget *view;			// The widget to display the buffer	
};
