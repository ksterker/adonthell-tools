/*
   $Id: gui_circle_events.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file gui_circle_events.h
 *
 * @author Kai Sterker
 * @brief Callbacks of the Edit Circle dialog
 */

#ifndef GUI_CIRCLE_EVENTS_H
#define GUI_CIRCLE_EVENTS_H

#include <gtk/gtkstyle.h>

void on_radio_button_pressed (GtkButton *button, gpointer user_data);
void on_circle_ok_button_pressed (GtkButton *button, gpointer user_data);

#endif // GUI_CIRCLE_EVENTS_H

