/*
 $Id: gui_mapview.cc,v 1.6 2009/05/18 21:21:22 ksterker Exp $
 
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

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include "backend/gtk/screen_gtk.h"

#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_mapview_events.h"
#include "map_data.h"

// ctor
GuiMapview::GuiMapview(GtkWidget *paned)
{
    // create drawing area for the graph
    Screen = gtk_drawing_area_new ();
    gtk_widget_set_size_request (GTK_WIDGET (Screen), 800, 600);
    gtk_paned_add2 (GTK_PANED (paned), Screen);
    gtk_widget_show (Screen);
    gtk_widget_grab_focus (Screen);
    
    // register our event callbacks
    gtk_signal_connect (GTK_OBJECT (Screen), "expose_event", (GtkSignalFunc) expose_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "configure_event", (GtkSignalFunc) configure_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "motion_notify_event", (GtkSignalFunc) motion_notify_event, this);
    /*
    gtk_signal_connect (GTK_OBJECT (Screen), "button_press_event", (GtkSignalFunc) button_press_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "button_release_event", (GtkSignalFunc) button_release_event, this);
    */
    gtk_signal_connect (GTK_OBJECT (GuiMapedit::window->getWindow ()), "key_press_event", (GtkSignalFunc) key_press_notify_event, this);
    
    gtk_widget_set_events (Screen, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);

    // create view
    View = new world::mapview (800, 600, &Renderer);
    View->set_map (NULL);
    
    // create the render target
    Target = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    CurObj = NULL;
}

// dtor
GuiMapview::~GuiMapview()
{
    delete View;
    delete Target;
}

// set map to render
void GuiMapview::setMap (MapData *area)
{
    View->set_map (area);
    View->set_position (area->x(), area->y());
    View->set_z (area->z());
    
    // display map coordinates of mouse pointer
    int x, y;
    gtk_widget_get_pointer (Screen, &x, &y);
    GuiMapedit::window->setLocation (x + area->x(), y + area->y(), area->z());
}

// redraw the screen
void GuiMapview::draw ()
{
    draw (0, 0, Screen->allocation.width, Screen->allocation.height);
}

// redraw the given part of the screen
void GuiMapview::draw (const int & sx, const int & sy, const int & l, const int & h)
{
    // reset target before drawing
    Target->set_window (Screen->window);
    Target->fillrect (sx, sy, l, h, 0xFF000000);
    
    // is there a map attached to the view at all?
    MapData *area = (MapData*) View->get_map();
    if (area != NULL)
    {
        // update position of map
        View->set_position (area->x() + sx, area->y() + sy);
        View->set_z (area->z());
        
        // render mapview to screen
        View->resize (l, h);
        View->draw (sx, sy, NULL, Target);
    }
}

// draw the given object at given offset
void GuiMapview::drawObject (world::chunk_info *obj, const int & ox, const int & oy)
{
    if (obj != NULL) 
    {
        // get location
        int x = obj->real_min().x();
        int y = obj->real_min().y() - obj->real_max().z();
        
        // get extend
        int l = obj->real_max().x() - x;
        int h = obj->real_max().y() - obj->real_min().z() - y;

        // draw
        draw (x - ox, y - oy, l, h);
        
        // schedule screen update
        GdkRectangle rect = { x - ox, y - oy, l, h };
        gdk_window_invalidate_rect (Screen->window, &rect, FALSE);
    }    
}

// update size of the view
void GuiMapview::resizeSurface (GtkWidget *widget)
{
    // set the size of the drawing area
    gtk_widget_set_size_request (GTK_WIDGET (Screen), widget->allocation.width, widget->allocation.height);

    // set the size of the actual map view
    View->resize (widget->allocation.width, widget->allocation.height);
}

// notification of mouse movement
void GuiMapview::mouseMoved (const GdkPoint * point)
{
    int ox = 0, oy = 0, oz = 0;
    MapData *area = (MapData*) View->get_map();
    if (area != NULL)
    {
        ox = area->x();
        oy = area->y();
        oz = area->z();
        
        GdkPoint p = { point->x + ox, point->y + oy };
        
        // find object that's being moused over
        std::list<world::chunk_info*> objects_under_mouse = area->objects_in_view (p.x, p.y, oz, 0, 0);
        world::chunk_info *obj = Renderer.findObjectBelowCursor (ox, oy + oz, &p, objects_under_mouse);
        if (obj != CurObj) 
        {
            // reset previously highlighted object
            drawObject (CurObj, ox, oy);
            // highlight new object
            drawObject (obj, ox, oy);
            // keep track of highlighted object
            CurObj = obj;
        }
    }
    
    // display map coordinates of mouse pointer
    GuiMapedit::window->setLocation (point->x + ox, point->y + oy, oz);
}

// delete object from map
void GuiMapview::deleteCurObj ()
{
    if (CurObj != NULL)
    {
        // remove object from map
        MapData *area = (MapData*) View->get_map();
        if (area->remove (*CurObj) != NULL)
        {
            // on success, redraw area containing object
            Renderer.clearSelection();
            drawObject (CurObj, area->x(), area->y());
            CurObj = NULL;
            
            // see if there's another object to select
            gint x, y;
            gtk_widget_get_pointer (Screen, &x, &y);
            GdkPoint mouseLoc = { x, y };
            mouseMoved (&mouseLoc);
        }
    }
}

// prepare everything for 'auto-scrolling' (TM) ;-)
bool GuiMapview::scrollingAllowed () const
{
    // is there a reason to scroll at all?
    MapData *area = (MapData*) View->get_map();
    return area != NULL;
}

// the actual scrolling
void GuiMapview::scroll ()
{
    // update area coordinates
    MapData *area = (MapData*) View->get_map();
    area->setX (area->x() - scroll_offset.x);
    area->setY (area->y() - scroll_offset.y);

    // scroll current window content
    gdk_window_scroll (GDK_WINDOW(Screen->window), scroll_offset.x, scroll_offset.y);
    
    // update map coordinates of mouse pointer
    int x, y;
    gtk_widget_get_pointer (Screen, &x, &y);
    GuiMapedit::window->setLocation (x + area->x(), y + area->y(), area->z());
}
