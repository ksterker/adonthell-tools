/*
   $Id: gui_modal_dialog.cc,v 1.1 2009/03/29 12:27:25 ksterker Exp $

   Copyright (C) 2002/2007 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file gui_modal_dialog.cc
 *
 * @author Kai Sterker
 * @brief Base class for modal dialog windows
 */

#include <gtk/gtk.h>
#include "gui_modal_dialog.h"

// ctor
GuiModalDialog::GuiModalDialog (GtkWindow *p)
{
    parent = p;
    pressedOK = false;
    window = NULL;
}

// clean up
GuiModalDialog::~GuiModalDialog ()
{
    // destroy the window
	if (window != NULL)
	{
		gtk_widget_destroy (GTK_WIDGET (window));
	}
}

// displays the dialog window
bool GuiModalDialog::run ()
{
    gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (parent));
    gtk_widget_show (GTK_WIDGET (window));
    gtk_main ();

    // return whether the OK button has been pressed
    return pressedOK;
}
