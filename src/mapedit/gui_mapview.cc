/*
 $Id: gui_mapview.cc,v 1.1 2009/03/29 12:27:26 ksterker Exp $
 
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
 * @file mapedit/gui_mapview.cc
 *
 * @author Kai Sterker
 * @brief Screen for the map.
 */

#include <gtk/gtk.h>

#include "backend/gtk/screen_gtk.h"

#include "gui_mapview.h"
#include "map_data.h"

// ctor
GuiMapview::GuiMapview(GtkWidget *paned)
{
    // create drawing area for the graph
    Screen = gtk_drawing_area_new ();
    gtk_drawing_area_size (GTK_DRAWING_AREA (Screen), 800, 600);
    gtk_paned_add2 (GTK_PANED (paned), Screen);
    gtk_widget_show (Screen);
    gtk_widget_grab_focus (Screen);
    
    /*
    // register our event callbacks
    gtk_signal_connect (GTK_OBJECT (Screen), "expose_event", (GtkSignalFunc) expose_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "configure_event", (GtkSignalFunc) configure_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "button_press_event", (GtkSignalFunc) button_press_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "button_release_event", (GtkSignalFunc) button_release_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "motion_notify_event", (GtkSignalFunc) motion_notify_event, this);
    // gtk_signal_connect (GTK_OBJECT (GuiDlgedit::window->getWindow ()), "key_press_event", (GtkSignalFunc) key_press_notify_event, this);
    */
    
    gtk_widget_set_events (Screen, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);

    // create view
    View = new world::mapview (800, 600);
}

// dtor
GuiMapview::~GuiMapview()
{
    delete View;
}

// set map to render
void GuiMapview::setMap (MapData *area)
{
    View->set_map (area);
}

// redraw the screen
void GuiMapview::draw ()
{
    // this is a GTK+ backed "screen" surface
    gfx::screen_surface_gtk *target = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    target->set_window (Screen->window);
    target->fillrect (0, 0, 800, 600, 0xFF000000);
 
    // TODO: position of view
    View->center_on (320, 240);

    // render mapview to screen
    View->draw (0, 0, NULL, target);
    
    // cleanup
    delete target;    
}

// update size of the view
void GuiMapview::resizeSurface (GtkWidget *widget)
{
    /*
    // delete the old backing pixmap
    if (Surface) gdk_pixmap_unref (surface);
    
    // create a new one with the proper size
    Surface = gdk_pixmap_new (widget->window, widget->allocation.width,
                              widget->allocation.height, -1);
    */
    
    // set the size of the drawing area
    // Screen->resize (widget->allocation.width, widget->allocation.height);
    
    // set the size of the actual map view
    View->resize (widget->allocation.width, widget->allocation.height);
}
