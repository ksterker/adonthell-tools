/*
 $Id: gui_scrollable.cc,v 1.2 2009/05/21 14:28:18 ksterker Exp $
 
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file common/gui_scrollable.cc
 *
 * @author Kai Sterker
 * @brief Basic functionality for automated scolling.
 */

#include <gtk/gtk.h>

#include "gui_scrollable.h"

/**
 * Callback for autoscrolling the graph view.
 *
 * Once 'auto-scrolling' is activated, this function is called every
 * 10th of a second until it returns FALSE
 */
int on_scroll_graph (gpointer data)
{
    int x, y;
    static int delay = 0;
    Scrollable *scroller = (Scrollable *) data;
    GtkWidget *widget = scroller->drawingArea ();
    
    // get the present cursor position (relative to the graph)    
    gtk_widget_get_pointer (widget, &x, &y);
    
    // stop scrolling if outside widget or too far from widget's border
    if (x < 0 || x > widget->allocation.width || 
        y < 0 || y > widget->allocation.height ||
        !scroller->isScrolling ())
    {
        scroller->stopScrolling ();
        delay = 0; 
        return FALSE;
    }
    
    // wait approx. 1 second before starting to scroll
    if (delay++ < 6) return TRUE;
    
    // move the view
    scroller->scroll ();
    
    return TRUE; 
}


// ctor
Scrollable::Scrollable()
{
    scrolling = false;
}

// dtor
Scrollable::~Scrollable()
{
}

// prepare everything for 'auto-scrolling' (TM) ;-)
void Scrollable::prepareScrolling (const GdkPoint & point)
{
    if (!scrollingAllowed ()) return;
    
    int scroll_x = 0;
    int scroll_y = 0;
    
    GtkWidget *widget = drawingArea ();

    // set scrolling offset and direction    
    if (point.x < 20) scroll_x = 15;
    if (point.y < 20) scroll_y = 15;
    if (point.x + 20 > widget->allocation.width) scroll_x = -15;
    if (point.y + 20 > widget->allocation.height) scroll_y = -15;
    
    // enable scrolling
    if (scroll_x || scroll_y)
    {
        scroll_offset.x = scroll_x;
        scroll_offset.y = scroll_y;
        
        if (!scrolling)
        {
            scrolling = true;
            
            // make sure that drawing has priority over scrolling
            g_timeout_add_full (G_PRIORITY_DEFAULT_IDLE, 100, on_scroll_graph, this, NULL);
        }
    }
    else scrolling = false;
}

// scrolling or not?
bool Scrollable::isScrolling () const
{ 
    return scrolling; 
}

// stop scrolling
void Scrollable::stopScrolling ()       
{ 
    scrolling = false; 
}
