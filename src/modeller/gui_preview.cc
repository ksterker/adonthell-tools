/*
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**  
 * @file modeller/gui_preview.cc
 *
 * @author Kai Sterker
 * @brief Where the model is displayed and assembled.
 */

#include <gtk/gtk.h>

#include <gfx/gfx.h>
#include <world/placeable_model.h>

#include "backend/gtk/screen_gtk.h"

#include "mdl_handle.h"
#include "gui_preview.h"

// Window resized
static gint configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GuiPreview *view = (GuiPreview *) data;
    
    // resize the drawing area
    view->resizeSurface (widget);
    
    return TRUE;
}

// Redraw the screen from the backing pixmap
static gint expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    GuiPreview *view = (GuiPreview *) data;
    
    // redraw part of the screen that has changed
    view->draw(event->area.x, event->area.y, event->area.width, event->area.height);
    
    return TRUE;
}

// Mouse moved over drawing area
gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    GuiPreview *view = (GuiPreview *) data;
    GdkPoint point = { event->x, event->y };
    
    // drag handle?
    if (event->state == GDK_BUTTON_PRESS_MASK)
    {
        view->handleDragged (&point);
    }
    else
    {
        if (view->isHandleDragged())
        {
            view->stopDragging();
        }
    }

    // highlight handles under cursor
    view->mouseMoved (&point);
        
    return FALSE;
}

// ctor
GuiPreview::GuiPreview (GtkWidget *drawing_area) : DrawingArea (drawing_area)
{
#ifdef __APPLE__
    // no need to use double buffering on OSX, but appears to be required elsewhere
    GTK_WIDGET_UNSET_FLAGS (GTK_WIDGET (DrawingArea), GTK_DOUBLE_BUFFERED);
#endif

    g_signal_connect (G_OBJECT (DrawingArea), "expose_event", G_CALLBACK(expose_event), this);
    g_signal_connect (G_OBJECT (DrawingArea), "configure_event", G_CALLBACK(configure_event), this);
    g_signal_connect (G_OBJECT (DrawingArea), "motion_notify_event", G_CALLBACK(motion_notify_event), this);
    // g_signal_connect (G_OBJECT (DrawingArea), "button_press_event", G_CALLBACK(button_press_event), this);
    // g_signal_connect (G_OBJECT (GuiMapedit::window->getWindow ()), "key_press_event", G_CALLBACK(key_press_notify_event), this);
    
    gtk_widget_set_events (DrawingArea, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);
    
    // create the render target
    Target = gfx::create_surface();    
    
    // no handle selected
    SelectedHandle = -1;
    PrevPos = NULL;
    Model = NULL;
    Shape = NULL;
}

// redraw the given part of the screen
void GuiPreview::draw (const int & sx, const int & sy, const int & l, const int & h)
{
    // get "screen" surface 
    gfx::screen_surface_gtk *s = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    s->set_drawable (DrawingArea->window);
    
    // set clipping rectangle
    gfx::drawing_area da (sx, sy, l, h);
    
    // draw target
    Target->draw (0, 0, &da, s);
}

// update size of the view
void GuiPreview::resizeSurface (GtkWidget *widget)
{
    // set the size of the drawing area
    gtk_widget_set_size_request (GTK_WIDGET (DrawingArea), widget->allocation.width, widget->allocation.height);
    
    // set size of the render target
    Target->resize (widget->allocation.width, widget->allocation.height);
    
    // update screen
    render ();
}    

// update screen
void GuiPreview::render ()
{
    render (0, 0, DrawingArea->allocation.width, DrawingArea->allocation.height);
}

// update part of the screen
void GuiPreview::render (const int & sx, const int & sy, const int & l, const int & h)
{
    // reset target before drawing
    Target->fillrect (sx, sy, l, h, 0xFF000000);

    if (Model != NULL)
    {
        // set clipping rectangle
        gfx::drawing_area da (sx, sy, l, h);
        
        // draw to target
        Renderer.render (Model, Handles, da, Target);
    }
        
    // schedule screen update
    GdkRectangle rect = { sx, sy, l, h };
    gdk_window_invalidate_rect (DrawingArea->window, &rect, FALSE);    
}

// set object being edited
void GuiPreview::setCurModel (world::placeable_model *model)
{
    Model = model;
    
    // no handle selected, initially
    SelectedHandle = -1;
    
    // update screen
    render ();
}

// set shape being edited
void GuiPreview::setCurShape (world::cube3 *shape)
{
    Renderer.setActiveShape (shape);

    // remember shape for updates
    Shape = shape;
    
    // no handle selected, initially
    SelectedHandle = -1;

    // update screen
    render ();
}

// notification of mouse movement
void GuiPreview::mouseMoved (const GdkPoint *point)
{
    if (Shape == NULL) return;
    
    // restrict drawing to view size
    gfx::drawing_area da (0, 0, Target->length()-1, Target->height()-1);
    // handle below cursor
    int curHandle = -1;
    
    for (int i = 0; i < MAX_HANDLES; i++)
    {
        GdkRectangle rect = { Handles[i].x, Handles[i].y, HANDLE_SIZE, HANDLE_SIZE };
        GdkRegion *region = gdk_region_rectangle (&rect);

        // printf ("[%i, %i] <-> [%i, %i]\n", Handles[i].x, Handles[i].y, point->x, point->y); 
        if (gdk_region_point_in (region, point->x, point->y))
        {
            curHandle = i;
            break;
        }
        
        // cleanup
        gdk_region_destroy (region);
    }

    // selection changed?
    if (curHandle == SelectedHandle) return;

    // was handle selected?
    if (SelectedHandle != -1)
    {
        // deselect handle
        Renderer.drawHandle (Handles[SelectedHandle], false, da, Target);
        // refresh screen
        GdkRectangle rect = { Handles[SelectedHandle].x, Handles[SelectedHandle].y, HANDLE_SIZE, HANDLE_SIZE };
        gdk_window_invalidate_rect (DrawingArea->window, &rect, FALSE);    
    }
            
    // is new handle selected?
    if (curHandle != -1)
    {
        // hightlight handle
        Renderer.drawHandle (Handles[curHandle], true, da, Target);
        // refresh screen
        GdkRectangle rect = { Handles[curHandle].x, Handles[curHandle].y, HANDLE_SIZE, HANDLE_SIZE };
        gdk_window_invalidate_rect (DrawingArea->window, &rect, FALSE);    
    }
            
    // update handle
    SelectedHandle = curHandle;        
}

// update shape size or position
void GuiPreview::handleDragged (GdkPoint *point)
{
    // dragging has just started
    if (PrevPos == NULL)
    {
        PrevPos = new GdkPoint();
        PrevPos->x = point->x;
        PrevPos->y = point->y;
    }
    
    // any change at all?
    if (PrevPos->x == point->x && PrevPos->y == point->y) 
    {
        return;
    }
    
    // list of points that will get updated
    int points[world::cube3::NUM_CORNERS];
    // the offset by which the points will be moved
    world::vector3<s_int16> offset;
    
    switch (SelectedHandle)
    {
        case POSITION:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;

            offset.set_x (point->x - PrevPos->x);
            offset.set_y (point->y - PrevPos->y);
            break;
        }
        case LENGTH:
        {
            // move the points on the cube's right side (x axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[4] = -1;
            
            offset.set_x (point->x - PrevPos->x);
            break;
        }
        case WIDTH:
        {
            // move the points on the cube's back side (y axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_BACK_LEFT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = -1;

            offset.set_y (point->y - PrevPos->y);
            break;
        }
        case HEIGHT:
        {
            // move the points on the cube's bottom side (z axis)
            points[0] = world::cube3::TOP_FRONT_LEFT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::TOP_BACK_RIGHT;
            points[3] = world::cube3::TOP_BACK_LEFT;
            points[4] = -1;
            
            offset.set_z (PrevPos->y - point->y);
            break;
        }
        default:
        {
            // no handle selected
            return;
        }
    }
    
    // now update the selected points
    for (int i = 0; i < world::cube3::NUM_CORNERS; i++)
    {
        if (points[i] == -1) break;
        
        world::vector3<s_int16> curPos = Shape->get_point (points[i]);
        Shape->set_point (points[i], curPos + offset);
    }
    
    // remember position for next iteration
    PrevPos->x = point->x;
    PrevPos->y = point->y;
    
    // update the bounding box and model
    Model->current_shape ()->remove_part (Shape);
    Shape->create_bounding_box ();
    Model->current_shape ()->add_part (Shape);
    
    // and finally the screen
    render ();
}
