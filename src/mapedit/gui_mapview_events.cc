/*
 $Id: gui_mapview_events.cc,v 1.1 2009/03/29 12:27:27 ksterker Exp $
 
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
 * @file mapedit/gui_mapview_events.cc
 *
 * @author Kai Sterker
 * @brief The Event callbacks used by the map view widget.
 */


#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "gui_mapview.h"

// Window resized
gint configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;

    // resize the drawing area
    view->resizeSurface (widget);

    // do a redraw
    view->draw ();
    
    return TRUE;
}

// Redraw the screen from the backing pixmap
gint expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;
    
    // TODO: full redraw for now, later draw from backing pixmap
    view->draw();
    
    /*
    gdk_draw_pixmap (widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
        view->pixmap (), event->area.x, event->area.y, event->area.x, event->area.y,
        event->area.width, event->area.height);
    */
    
    return FALSE;
}

/*
// Mouse-button pressed on Drawing Area
gint button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;
    DlgPoint point ((int) event->x, (int) event->y);

    switch (event->button)
    {
        // Middle button pressed
        case 2:
        {
            // ignore edit command if in preview mode
            if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                break;
            
            // If nothing selected, see if we're over a node
            if (view->mode () == IDLE)
                if (!view->selectNode (point))
                    // create a submodule, if we aren't
                    view->newModule (point);
     
            // Edit node
            if (view->mode () == NODE_SELECTED)
                view->editNode ();
            
            break;
        }
        
        // Right button pressed
        case 3:
        {
            // if something selected -> deselect
            if (view->mode () == NODE_SELECTED)
                view->deselectNode ();
            
            break;
        }

        default: break;
    }
    
    return TRUE;
}

// Mouse moved over drawing area
gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;
    DlgPoint point ((int) event->x, (int) event->y);

    // scroll the view if necessary (this has to happen before 
    // anything else, as the next method(s) change 'point'.
    view->prepareScrolling (point);

    // Dragging dialogue nodes
    if (event->state == GDK_BUTTON_PRESS_MASK)
    {
        // don't allow dragging if in preview mode
        if (GuiDlgedit::window->mode () == L10N_PREVIEW)
            return FALSE;
        
        // no node being dragged so far -> start dragging
        if (view->mode () != NODE_DRAGGED)
            view->prepareDragging (point);
        // otherwise continue moving
        else
            view->drag (point);
    }
    // highlight nodes under the cursor and display their 'tooltip'
    else view->mouseMoved (point);

    return FALSE;
}

// Mouse-button released on Drawing Area
gint button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;
    DlgPoint point ((int) event->x, (int) event->y);
    
    // Left button released
    if (event->button == 1)
    {
        switch (view->mode ())
        {
            // nothing selected
            case IDLE:
            {
                // select the node under the cursor, if any
                if (!view->selectNode (point))
                    // otherwise create a new circle at that position
                    if (GuiDlgedit::window->mode () != L10N_PREVIEW)
                        view->newCircle (point);
                
                break;
            }

            // node selected
            case NODE_SELECTED:
            {
                // ignore edit command if in preview mode
                if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                    break;

                // try to create a new link between two nodes
                view->newArrow (point);
                break;
            }
                        
            // node dragged
            case NODE_DRAGGED:
            {
                // stop dragging
                view->stopDragging (point);
                break;
            }
                    
            default: break;
        }
    }
    return TRUE;
}

// Key pressed
guint key_press_notify_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
    GuiMapview *view = (GuiMapview *) user_data;

    switch (event->keyval)
    {
        // select parent node
        case GDK_Up:
        {
            if (view->selectParent ()) view->centerNode ();
            break;
        }
        
        // select child node
        case GDK_Down:
        {
            if (view->selectChild ()) view->centerNode ();
            break;
        }
        
        // select sibling to the left
        case GDK_Left:
        {
            if (view->selectSibling (PREV)) view->centerNode ();
            break;
        }
        
        // select sibling to the right
        case GDK_Right:
        {
            if (view->selectSibling (NEXT)) view->centerNode ();
            break;
        }
        
        // edit selected node
        case GDK_Return:
        {
            int x, y;

            // ignore edit command if in preview mode
            if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                break;            

            // get cursoer position
            gtk_widget_get_pointer (view->drawingArea (), &x, &y);
            DlgPoint point (x, y);                              

            // If nothing selected, see if we're over a node
            if (view->mode () == IDLE)
                if (!view->selectNode (point))
                    // create a submodule, if we aren't
                    view->newModule (point);

            // Edit node
            if (view->mode () == NODE_SELECTED)
                view->editNode ();

            break;
        }
        
        // deselect Node
        case GDK_Escape:
        {
            view->deselectNode ();
            break;
        }
        
        // delete node
        case GDK_Delete:
        {
            // ignore delete command if in preview mode
            if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                break;            

            view->deleteNode ();
            break;
        }
    }
    return TRUE;
}


// Once 'auto-scrolling' is activated, this function is called every
// 10th of a second until it returns FALSE
int on_scroll_graph (gpointer data)
{
    int x, y;
    static int delay = 0;
    GuiMapview *view = (GuiMapview *) data;
    GtkWidget *widget = view->drawingArea ();

    // get the present cursor position (relative to the view)    
    gtk_widget_get_pointer (widget, &x, &y);
    
    // stop scrolling if outside widget or too far from widget's border
    if (x < 0 || x > widget->allocation.width || 
        y < 0 || y > widget->allocation.height ||
        !view->isScrolling ())
    {
        view->stopScrolling ();
        delay = 0; 
        return FALSE;
    }
    
    // wait approx. 1 second before starting to scroll
    if (delay++ < 6) return TRUE;
    
    // move the view
    view->scroll ();
    
    return TRUE; 
}
*/
