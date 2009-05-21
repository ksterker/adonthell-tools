/*
 $Id: gui_mapview_events.cc,v 1.5 2009/05/21 14:28:18 ksterker Exp $
 
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
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include "gui_mapview.h"

// Window resized
gint configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;

    // resize the drawing area
    view->resizeSurface (widget);
    
    return TRUE;
}

// Redraw the screen from the backing pixmap
gint expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;
    
    // redraw part of the screen that has changed
    view->draw(event->area.x, event->area.y, event->area.width, event->area.height);
    
    return TRUE;
}

// Mouse-button pressed on Drawing Area
gint button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    GuiMapview *view = (GuiMapview *) data;

    switch (event->button)
    {
        // Left button pressed
        case 1:
        {
            // no object picked -> grab
            view->selectCurObj ();
            
            // if object picked -> add to map 
            break;
        }
        // Right button pressed
        case 3:
        {
            // if object picked -> release
            view->releaseObject ();
            
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
    GdkPoint point = { event->x, event->y };

    // scroll the view if necessary (this has to happen before 
    // anything else, as the next method(s) change 'point'.
    view->prepareScrolling (point);

    // highlight nodes under the cursor and display their 'tooltip'
    view->mouseMoved (&point);

    return FALSE;
}

/*
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
*/
// Key pressed
guint key_press_notify_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
    GuiMapview *view = (GuiMapview *) user_data;

    switch (event->keyval)
    {
        /*
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
        */
        
        // deselect Node
        case GDK_Escape:
        {
            view->releaseObject ();
            break;
        }
        // delete node
        case GDK_BackSpace: // fall through
        case GDK_Delete:
        {
            view->deleteCurObj ();
            break;
        }
            
        default:
        {
            return FALSE;
        }
    }
    
    return TRUE;
}

