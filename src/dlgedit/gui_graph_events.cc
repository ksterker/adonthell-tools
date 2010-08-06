/*
   $Id: gui_graph_events.cc,v 1.2 2009/04/03 22:00:35 ksterker Exp $

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
 * @file gui_graph_events.cc
 *
 * @author Kai Sterker
 * @brief Event-callbacks for the dialogue graph view
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "gui_dlgedit.h"

// Window resized
gint configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GuiGraph *graph = (GuiGraph *) data;

    // resize the drawing area
    graph->resizeSurface (widget);

    // do a redraw
    graph->draw ();
    
    // redraw the preview
    GuiDlgedit::window->list()->draw ();

    return TRUE;
}

// Redraw the screen from the backing pixmap
gint expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    GuiGraph *graph = (GuiGraph *) data;

    gdk_draw_pixmap (widget->window, widget->style->fg_gc[GTK_WIDGET_STATE (widget)],
        graph->pixmap (), event->area.x, event->area.y, event->area.x, event->area.y,
        event->area.width, event->area.height);

    return FALSE;
}

// Mouse-button pressed on Drawing Area
gint button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GuiGraph *graph = (GuiGraph *) data;
    DlgPoint point ((int) event->x, (int) event->y);

#ifdef __APPLE__
    // simulate right click on OSX
    if (event->state & GDK_CONTROL_MASK)
    {
        event->button = 3;
    }
#endif
    
    switch (event->button)
    {
        // Middle button pressed
        case 2:
        {
            // ignore edit command if in preview mode
            if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                break;
            
            // If nothing selected, see if we're over a node
            if (graph->mode () == IDLE)
                if (!graph->selectNode (point))
                    // create a submodule, if we aren't
                    graph->newModule (point);
     
            // Edit node
            if (graph->mode () == NODE_SELECTED)
                graph->editNode ();
            
            break;
        }
        
        // Right button pressed
        case 3:
        {
            // if something selected -> deselect
            if (graph->mode () == NODE_SELECTED)
                graph->deselectNode ();
            
            break;
        }

        default: break;
    }
    
    return TRUE;
}

// Mouse moved over drawing area
gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    GuiGraph *graph = (GuiGraph *) data;
    DlgPoint point ((int) event->x, (int) event->y);

    // scroll the graph if necessary (this has to happen before 
    // anything else, as the next method(s) change 'point'.
    graph->prepareScrolling (point);

    // Dragging dialogue nodes
    if (event->state & GDK_BUTTON1_MASK)
    {
        // don't allow dragging if in preview mode
        if (GuiDlgedit::window->mode () == L10N_PREVIEW)
            return FALSE;
        
        // no node being dragged so far -> start dragging
        if (graph->mode () != NODE_DRAGGED)
            graph->prepareDragging (point);
        // otherwise continue moving
        else
            graph->drag (point);
    }
    // highlight nodes under the cursor and display their 'tooltip'
    else graph->mouseMoved (point);

    return FALSE;
}

// Mouse-button released on Drawing Area
gint button_release_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GuiGraph *graph = (GuiGraph *) data;
    DlgPoint point ((int) event->x, (int) event->y);
    
#ifdef __APPLE__
    // simulate right click on OSX
    if (event->state & GDK_CONTROL_MASK)
    {
        event->button = 3;
    }
#endif    
    
    // Left button released
    if (event->button == 1)
    {
        switch (graph->mode ())
        {
            // nothing selected
            case IDLE:
            {
                // select the node under the cursor, if any
                if (!graph->selectNode (point))
                    // otherwise create a new circle at that position
                    if (GuiDlgedit::window->mode () != L10N_PREVIEW)
                        graph->newCircle (point);
                
                break;
            }

            // node selected
            case NODE_SELECTED:
            {
                // ignore edit command if in preview mode
                if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                    break;

                // try to create a new link between two nodes
                graph->newArrow (point);
                break;
            }
                        
            // node dragged
            case NODE_DRAGGED:
            {
                // stop dragging
                graph->stopDragging (point);
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
    GuiGraph *graph = (GuiGraph *) user_data;

    switch (event->keyval)
    {
        // select parent node
        case GDK_Up:
        {
            if (graph->selectParent ()) graph->centerNode ();
            break;
        }
        
        // select child node
        case GDK_Down:
        {
            if (graph->selectChild ()) graph->centerNode ();
            break;
        }
        
        // select sibling to the left
        case GDK_Left:
        {
            if (graph->selectSibling (PREV)) graph->centerNode ();
            break;
        }
        
        // select sibling to the right
        case GDK_Right:
        {
            if (graph->selectSibling (NEXT)) graph->centerNode ();
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
            gtk_widget_get_pointer (graph->drawingArea (), &x, &y);
            DlgPoint point (x, y);                              

            // If nothing selected, see if we're over a node
            if (graph->mode () == IDLE)
                if (!graph->selectNode (point))
                    // create a submodule, if we aren't
                    graph->newModule (point);

            // Edit node
            if (graph->mode () == NODE_SELECTED)
                graph->editNode ();

            break;
        }
        
        // deselect Node
        case GDK_Escape:
        {
            graph->deselectNode ();
            break;
        }
        
        // delete node
        case GDK_BackSpace: // fall through
        case GDK_Delete:
        {
            // ignore delete command if in preview mode
            if (GuiDlgedit::window->mode () == L10N_PREVIEW)
                break;            

            graph->deleteNode ();
            break;
        }
        
        // key not consumed
        default:
        {
            return FALSE;
        }
    }
    
    return TRUE;
}
