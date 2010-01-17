/*
 $Id: gui_mapview_events.cc,v 1.6 2009/05/24 13:40:28 ksterker Exp $
 
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
            if (view->getSelectedObject() == NULL)
            {
                // no object picked -> grab
                view->selectCurObj ();
            }
            else
            {
                // if object picked -> add to map 
                view->placeCurObj ();
            }
            break;
        }
        // Middle button pressed
        case 2:
        {
            view->editCurObject ();
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

// Key pressed
guint key_press_notify_event (GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
    GuiMapview *view = (GuiMapview *) user_data;

    switch (event->keyval)
    {
	    // scroll up
	    case GDK_Up:
	    {
	    	if (view->scrollingAllowed()) 
	    	{
	    		view->setScrollOffset(0, 40);
	    		view->scroll();
	    	}
	        break;
	    }
	    // scroll down
	    case GDK_Down:
	    {
	    	if (view->scrollingAllowed()) 
	    	{
	    		view->setScrollOffset(0, -40);
	    		view->scroll();
	    	}
	        break;
	    }
	    // scroll left
	    case GDK_Left:
	    {
	    	if (view->scrollingAllowed()) 
	    	{
	    		view->setScrollOffset(40, 0);
	    		view->scroll();
	    	}
	        break;
	    }
	    // scroll right
	    case GDK_Right:
	    {
	    	if (view->scrollingAllowed())
	    	{
	    		view->setScrollOffset(-40, 0);
	    		view->scroll();
	    	}
	        break;
	    }
        // increase object z position
        case GDK_plus:
        {
            view->updateHeight (event->state & GDK_CONTROL_MASK ? 10 : 1);
            break;
        }
        // decrease object z position
        case GDK_minus:
        {
            view->updateHeight (event->state & GDK_CONTROL_MASK ? -10 : -1);
            break;
        }
        // edit highlighted object
        case GDK_Return:
        {
            view->editCurObject ();
            break;
        }
        // deselect object
        case GDK_Escape:
        {
            view->releaseObject ();
            break;
        }
        // delete highlighted node
        case GDK_BackSpace: // fall through
        case GDK_Delete:
        {
            view->deleteCurObj ();
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

