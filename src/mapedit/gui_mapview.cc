/*
 $Id: gui_mapview.cc,v 1.7 2009/05/21 14:28:18 ksterker Exp $
 
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

#include "gfx/gfx.h"
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
    
    GTK_WIDGET_UNSET_FLAGS (GTK_WIDGET (Screen), GTK_DOUBLE_BUFFERED);
    
    gtk_widget_set_size_request (GTK_WIDGET (Screen), 800, 600);
    gtk_paned_add2 (GTK_PANED (paned), Screen);
    gtk_widget_show (Screen);
    gtk_widget_grab_focus (Screen);
    
    // register our event callbacks
    gtk_signal_connect (GTK_OBJECT (Screen), "expose_event", (GtkSignalFunc) expose_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "configure_event", (GtkSignalFunc) configure_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "motion_notify_event", (GtkSignalFunc) motion_notify_event, this);
    gtk_signal_connect (GTK_OBJECT (Screen), "button_press_event", (GtkSignalFunc) button_press_event, this);
    /*
    gtk_signal_connect (GTK_OBJECT (Screen), "button_release_event", (GtkSignalFunc) button_release_event, this);
    */
    gtk_signal_connect (GTK_OBJECT (GuiMapedit::window->getWindow ()), "key_press_event", (GtkSignalFunc) key_press_notify_event, this);
    
    gtk_widget_set_events (Screen, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);

    // create view
    View = new world::mapview (800, 600, &Renderer);
    View->set_map (NULL);
    
    // create the render target
    Target = gfx::create_surface();
    
    // create the overlay
    Overlay = gfx::create_surface ();
    Overlay->set_alpha (255, true);
    
    // Memeber intialization
    CurObj = NULL;
    DrawObj = NULL;
}

// dtor
GuiMapview::~GuiMapview()
{
    delete View;
    delete Target;
    delete Overlay;
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
    
    // draw map
    render();
}

// redraw the given part of the screen
void GuiMapview::draw (const int & sx, const int & sy, const int & l, const int & h)
{
    // get "screen" surface 
    gfx::screen_surface_gtk *s = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    s->set_window (Screen->window);
    
    // set clipping rectangle
    gfx::drawing_area da (sx, sy, l, h);
    
    // draw mapview
    Target->draw (0, 0, &da, s);
    
    // draw overlay
    Overlay->draw (0, 0, &da, s);
}

// render the whole map view
void GuiMapview::render ()
{
    render (0, 0, Screen->allocation.width, Screen->allocation.height);
}

// render part of the map view
void GuiMapview::render (const int & sx, const int & sy, const int & l, const int & h)
{
    // reset target before drawing
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
    
    // schedule screen update
    GdkRectangle rect = { sx, sy, l, h };
    gdk_window_invalidate_rect (Screen->window, &rect, FALSE);    
}

// render specific object at given offset
void GuiMapview::renderObject (world::chunk_info *obj)
{
    if (obj != NULL) 
    {
        // get object bbox
        int x, y, l, h;
        getObjectExtend (obj, x, y, l, h);
        
        // get map offset
        MapData *area = (MapData*) View->get_map();
        
        // draw
        render (x - area->x(), y - area->y(), l, h);
    }    
}

// update size of the view
void GuiMapview::resizeSurface (GtkWidget *widget)
{
    // set the size of the drawing area
    gtk_widget_set_size_request (GTK_WIDGET (Screen), widget->allocation.width, widget->allocation.height);

    // set the size of the actual map view
    View->resize (widget->allocation.width, widget->allocation.height);
    
    // set size of the map view render target
    Target->resize (widget->allocation.width, widget->allocation.height);
    
    // set the size of the overlay
    Overlay->resize (widget->allocation.width, widget->allocation.height);
    Overlay->fillrect (0, 0, widget->allocation.width, widget->allocation.height, 0);
    
    // redraw everything
    render ();
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
        
        if (DrawObj == NULL)
        {
            GdkPoint p = { point->x + ox, point->y + oy };
            
            // find object that's being moused over
            std::list<world::chunk_info*> objects_under_mouse = area->objects_in_view (p.x, p.y, oz, 0, 0);
            world::chunk_info *obj = Renderer.findObjectBelowCursor (ox, oy + oz, &p, objects_under_mouse);
            if (obj != CurObj) 
            {
                // reset previously highlighted object
                renderObject (CurObj);
                // highlight new object
                renderObject (obj);
                // keep track of highlighted object
                CurObj = obj;
            }
        }
        else
        {
            GdkRectangle rect1 = { DrawObjPos.x(), DrawObjPos.y(), DrawObjSurface->length(), DrawObjSurface->height() };
            GdkRegion *region = gdk_region_rectangle (&rect1);
            
            // erase at previous position
            Overlay->fillrect (DrawObjPos.x(), DrawObjPos.y(), DrawObjSurface->length(), DrawObjSurface->height(), 0x00FFFFFF);
            
            // store new position
            DrawObjPos = world::vector3<s_int32> (point->x, point->y, 0);
            
            GdkRectangle rect2 = { point->x, point->y, DrawObjSurface->length(), DrawObjSurface->height() };
            gdk_region_union_with_rect (region, &rect2);
            
            // draw at new position
            DrawObjSurface->draw (DrawObjPos.x(), DrawObjPos.y(), NULL, Overlay);
            
            gdk_window_invalidate_region (Screen->window, region, FALSE);
            gdk_region_destroy (region);
        }
    }
    
    // display map coordinates of mouse pointer
    GuiMapedit::window->setLocation (point->x + ox, point->y + oy, oz);
}

// pick currently highlighted object for drawing
void GuiMapview::selectCurObj ()
{
    if (CurObj != NULL)
    {
        int x, y, l, h;
        DrawObj = CurObj->get_entity();
        getObjectExtend (CurObj, x, y, l, h);
        
        // create a surface onto which to draw the picked object
        DrawObjSurface = gfx::create_surface ();
        DrawObjSurface->set_alpha (128, true);
        DrawObjSurface->resize (l, h);
        DrawObjSurface->fillrect (0, 0, l, h, 0x00000000);

        // properly render the object
        std::list <world::chunk_info*> object_list;
        object_list.push_back (CurObj);
        gfx::drawing_area da (0, 0, l, h);
        world::debug_renderer renderer (true);
        renderer.render (-x, -y, object_list, da, DrawObjSurface);
        
        // give tile some contrast
        DrawObjSurface->set_brightness (150);
    }
}

// drop object currently picked for drawing
void GuiMapview::releaseObject ()
{
    if (DrawObj != NULL)
    {
        // erase at previous position
        GdkRectangle rect = { DrawObjPos.x(), DrawObjPos.y(), DrawObjSurface->length(), DrawObjSurface->height() };
        Overlay->fillrect (DrawObjPos.x(), DrawObjPos.y(), DrawObjSurface->length(), DrawObjSurface->height(), 0);
        gdk_window_invalidate_rect (Screen->window, &rect, FALSE);

        // cleanup
        delete DrawObjSurface;
        DrawObjSurface = NULL;
        DrawObj = NULL;
        
        // highlight new object
        highlightObject();
    }
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
            renderObject (CurObj);
            CurObj = NULL;
            
            // see if there's another object to select
            highlightObject();
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

    // rendering the whole mapview is less performant than doing
    // gdk_window_scroll (GDK_WINDOW(Screen->window), scroll_offset.x, scroll_offset.y);
    // but it appears to be the only way to prevent artifacts from appearing
    
    // render at new position
    render ();
    
    // update map coordinates of mouse pointer
    int x, y;
    gtk_widget_get_pointer (Screen, &x, &y);
    GuiMapedit::window->setLocation (x + area->x(), y + area->y(), area->z());
}

// highlight object under mouse, if any
void GuiMapview::highlightObject ()
{
    gint x, y;
    gtk_widget_get_pointer (Screen, &x, &y);
    GdkPoint mouseLoc = { x, y };
    mouseMoved (&mouseLoc);
}

// calculate rendered object's size
void GuiMapview::getObjectExtend (world::chunk_info *obj, int & x, int & y, int & l, int & h)
{
    // get location
    x = obj->real_min().x();
    y = obj->real_min().y() - obj->real_max().z();
    
    // get extend
    l = obj->real_max().x() - x;
    h = obj->real_max().y() - obj->real_min().z() - y;
}
