/*
 $Id: gui_mapview_events.h,v 1.3 2009/05/18 21:21:23 ksterker Exp $
 
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
 * @file mapedit/gui_mapview_events.h
 *
 * @author Kai Sterker
 * @brief Event-callbacks for the map view
 */

#ifndef GUI_MAPVIEW_EVENTS_H
#define GUI_MAPVIEW_EVENTS_H

#include <gtk/gtkstyle.h>
#include <gdk/gdktypes.h>

/**
 * Callback notifying that the widget's size has changed.
 */
gint configure_event (GtkWidget *, GdkEventConfigure *, gpointer);
/**
 * Callback indicating that the widget needs to be redrawn.
 */
gint expose_event (GtkWidget *, GdkEventExpose *, gpointer);
/**
 * Callback indicating that a mouse button has been pressed.
 */
// gint button_press_event (GtkWidget *, GdkEventButton *, gpointer);
/**
 * Callback indicating that a mouse button has been released.
 */
// gint button_release_event (GtkWidget *, GdkEventButton *, gpointer);
/**
 * Callback indicating that the mouse has been moved.
 */
gint motion_notify_event (GtkWidget *, GdkEventMotion *, gpointer);
/**
 * Callback indicating that a key has been pushed.
 */
guint key_press_notify_event (GtkWidget *, GdkEventKey *, gpointer);

#endif // GUI_MAPVIEW_EVENTS_H
