/*
   $Id: gui_mapedit_events.h,v 1.1 2009/03/29 12:27:26 ksterker Exp $
   
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
 * @file mapedit/gui_mapedit_events.h
 *
 * @author Kai Sterker
 * @brief The Events triggered via the Map Editor's main window
 */

#ifndef GUI_MAPEDIT_EVENTS_H
#define GUI_MAPEDIT_EVENTS_H

void on_widget_destroy (GtkWidget *, gpointer);

// Menu Callbacks
void on_file_new_activate (GtkMenuItem *, gpointer);
void on_file_load_activate (GtkMenuItem *, gpointer);
// void on_file_load_recent_activate (GtkMenuItem *, gpointer);
void on_file_save_activate (GtkMenuItem *, gpointer);
void on_file_save_as_activate (GtkMenuItem *, gpointer);
// void on_file_revert_activate (GtkMenuItem *, gpointer);
// void on_file_close_activate (GtkMenuItem *, gpointer);

// Statusbar callbacks
// gboolean on_display_help (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);
// gboolean on_clear_help (GtkWidget *widget, GdkEventCrossing *event, gpointer user_data);

#endif // GUI_MAPEDIT_EVENTS_H
