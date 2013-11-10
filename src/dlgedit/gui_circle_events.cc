/*
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
 * @file gui_circle_events.cc
 *
 * @author Kai Sterker
 * @brief Callbacks of the Edit Circle dialog
 */

#include "gui_circle_events.h"
#include "gui_circle.h"

// Colorize the dialogue text according to the character it is assigned to
void on_radio_button_pressed (GtkButton * button, gpointer user_data)
{
    GtkWidget *entry = (GtkWidget *) user_data;
    GtkTextBuffer *buf = gtk_text_view_get_buffer (GTK_TEXT_VIEW (entry));
    
    int type = GPOINTER_TO_INT (g_object_get_data (G_OBJECT (button), "type"));
    GdkColor color = { 0, 0, 0, 0 };

    switch (type)
    {
        case PLAYER:
            color.blue = 35000;
            break;
        case NARRATOR:
            color.green = 27300;
            color.blue = 15600;
            break;
    }

    // Change the color of the text entry
    gtk_widget_modify_text (entry, GTK_STATE_NORMAL, &color);

    // remember the type of the text
    g_object_set_data (G_OBJECT (buf), "type", GINT_TO_POINTER (type));
}

// close the dialog and keep all changes
void on_circle_ok_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiCircle *dialog = (GuiCircle *) user_data;

    dialog->applyChanges ();
    dialog->okButtonPressed (true);

    // clean up
    gtk_main_quit ();
}

// close dialog and dismiss all changes
void on_circle_cancel_button_pressed (GtkButton * button, gpointer user_data)
{
    // clean up
    gtk_main_quit ();
}

