/*
 $Id: gui_mapview.cc,v 1.2 2009/04/03 22:00:47 ksterker Exp $
 
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
    // gtk_signal_connect (GTK_OBJECT (GuiDlgedit::window->getWindow ()), "key_press_event", (GtkSignalFunc) key_press_notify_event, this);
    */
    
    gtk_widget_set_events (Screen, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);

    // create view
    View = new world::mapview (800, 600, &Renderer);
    View->set_map (NULL);
    
    // create the render target
    Target = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
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
    View->set_position (0, 0);
}

// redraw the screen
void GuiMapview::draw ()
{
    // reset target before drawing
    Target->set_window (Screen->window);
    Target->fillrect (0, 0, Screen->allocation.width, Screen->allocation.height, 0xFF000000);
 
    // is there a map attached to the view at all?
    MapData *area = (MapData*) View->get_map();
    if (area != NULL)
    {
        // update position of view
        View->set_position (area->x(), area->y());
        View->set_z (area->z());
        
        // render mapview to screen
        View->draw (0, 0, NULL, Target);
    }
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
        
        std::list<world::chunk_info*> objects_under_mouse = area->objects_in_view (point->x + ox, point->y + oy, oz, 0, 0);
        world::chunk_info *obj = Renderer.findObjectBelowCursor (ox, oy, point, objects_under_mouse);
        // FIXME: highlightObject (obj);
        if (obj != NULL && obj != CurObj) 
        {
            draw ();
            CurObj = obj;
        }
    }
    
    // display map coordinates of mouse pointer
    GuiMapedit::window->setLocation (point->x + ox, point->y + oy, oz);
}

// FIXME: highlighting only the object under the cursor does not work yet
void GuiMapview::highlightObject (world::chunk_info *obj)
{
    if (obj != NULL && obj != CurObj) 
    {
        // get map
        MapData *area = (MapData*) View->get_map();
        
        // get offset
        int ox = area->x();
        int oy = area->y();
        int oz = area->z();
        
        // get location
        int x = obj->Min.x() + ox;
        int y = obj->Min.y() + oy;
        
        // get extend
        int l = obj->Max.x() - obj->Min.x();
        int h = obj->Max.y() - obj->Max.y();
        
        // gfx::drawing_area da (x, y, l, h);
        gfx::drawing_area da (0, 0, 800, 600);
        
        // get area covered by newly highlighted object
        std::list<world::chunk_info*> highlighted_objects = area->objects_in_view (x+1, y+1, oz, l-1, h-1);
        Renderer.render (ox, oy, highlighted_objects, da, Target);
        
        CurObj = obj;
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
    MapData *area = (MapData*) View->get_map();
    area->setX (area->x() - scroll_offset.x);
    area->setY (area->y() - scroll_offset.y);
    
    draw ();
}
