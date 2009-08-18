/*
 $Id: mdl_cmdline.h,v 1.1 2009/03/29 12:27:27 ksterker Exp $
 
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


// ctor
GuiPreview::GuiPreview (GtkWidget *drawing_area) : DrawingArea (drawing_area), Model (NULL)
{
#ifdef __APPLE__
    // no need to use double buffering on OSX, but appears to be required elsewhere
    GTK_WIDGET_UNSET_FLAGS (GTK_WIDGET (DrawingArea), GTK_DOUBLE_BUFFERED);
#endif

    g_signal_connect (G_OBJECT (DrawingArea), "expose_event", G_CALLBACK(expose_event), this);
    g_signal_connect (G_OBJECT (DrawingArea), "configure_event", G_CALLBACK(configure_event), this);
    // g_signal_connect (G_OBJECT (DrawingArea), "motion_notify_event", G_CALLBACK(motion_notify_event), this);
    // g_signal_connect (G_OBJECT (DrawingArea), "button_press_event", G_CALLBACK(button_press_event), this);
    // g_signal_connect (G_OBJECT (GuiMapedit::window->getWindow ()), "key_press_event", G_CALLBACK(key_press_notify_event), this);
    
    gtk_widget_set_events (DrawingArea, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);
    
    // create the render target
    Target = gfx::create_surface();    
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
        gfx::sprite *sprt = Model->get_sprite();
        if (sprt != NULL)
        {
            // set clipping rectangle
            gfx::drawing_area da (sx, sy, l, h);
            
            // center on screen
            s_int16 x = (Target->length() - sprt->length()) / 2;
            s_int16 y = (Target->height() - sprt->height()) / 2;
            
            // draw
            sprt->draw (x, y, &da, Target);
        }
    }
        
    // schedule screen update
    GdkRectangle rect = { sx, sy, l, h };
    gdk_window_invalidate_rect (DrawingArea->window, &rect, FALSE);    
}

// set object being edited
void GuiPreview::setCurModel (world::placeable_model *model)
{
    Model = model;
    
    // update screen
    render ();
}

