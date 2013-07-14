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

/// the area around the view sensitive to scrolling
#define ACTIVE_BORDER 40
/// amount of pixels the view is moved per iteration
#define SCROLL_OFFSET 15

/// width and length of the cursor
#define SCROLL_CURSOR_SIZE (24)
/// a cursor to indicate scrolling
static const guint8 SCROLL_CURSOR_pixel_data[24 * 24 * 4 + 1] =
("\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\0\0\0\377\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\0\0\0\377\200\200\200\377\200\200\200\377\0\0\0\377\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\200\200\200\377\377\377\377"
 "\377\377\377\377\377\200\200\200\377\0\0\0\377\377\377\377\0\377\377\377\0"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\200"
 "\200\200\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\200\200\200\377\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0"
 "\0\377\0\0\0\377\0\0\0\377\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\0\0\0\377\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\0\0\0\377\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\200\200"
 "\200\377\0\0\0\344\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377"
 "\200\200\200\377\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0"
 "\377\200\200\200\377\377\377\377\377\0\0\0\377\377\377\377\0\377\377\377\0"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\0\0\0\377"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\0\0\0\377\377\377\377\377\200\200\200\377\0\0\0\377\377\377\377"
 "\0\0\0\0\377\200\200\200\377\377\377\377\377\377\377\377\377\0\0\0\377\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377"
 "\377\377\377\377\377\377\377\377\0\0\0\377\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\377\377\377\377\377\377\377"
 "\377\200\200\200\377\0\0\0\377\0\0\0\377\200\200\200\377\377\377\377\377\377"
 "\377\377\377\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\0\0\0\377\377\377\377\377\377\377\377\377\0\0\0\377\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\377"
 "\377\377\377\377\377\377\377\200\200\200\377\0\0\0\377\377\377\377\0\0\0\0"
 "\377\200\200\200\377\377\377\377\377\0\0\0\377\377\377\377\0\377\377\377\0"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\0\0\0\377"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\0\0\0\377\377\377\377\377\200\200\200\377\0\0\0\377\377\377\377"
 "\0\377\377\377\0\377\377\377\0\0\0\0\377\200\200\200\377\0\0\0\302\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\200\200\200\377\0\0\0\377"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0"
 "\0\350\0\0\0\345\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\0"
 "\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\0\0\0\377\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377"
 "\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\0\0\0\377\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\0\0\0\377\200\200\200\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\200\200\200\377\0\0\0\377"
 "\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\0\0\0\377\200\200\200\377\377\377\377\377\377\377\377\377"
 "\200\200\200\377\0\0\0\377\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377"
 "\200\200\200\377\200\200\200\377\0\0\0\377\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377"
 "\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\0\0\0\377\0\0\0\377\377\377\377\0\377"
 "\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377"
 "\377\0\377\377\377\0\377\377\377\0\377\377\377\0\377\377\377\0");

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

    // check that parent window is active
    GtkWidget *window = gtk_widget_get_ancestor (widget, GTK_TYPE_WINDOW);
    if (window != NULL && !gtk_window_is_active (GTK_WINDOW(window)))
    {
        scroller->stopScrolling ();
        delay = 0; 
        return FALSE;
    }
    
    // get the present cursor position (relative to the graph)    
    gtk_widget_get_pointer (widget, &x, &y);

    GtkAllocation allocation;
    gtk_widget_get_allocation (widget, &allocation);

    // stop scrolling if outside widget or too far from widget's border
    if (x < 0 || x > allocation.width ||
        y < 0 || y > allocation.height ||
        !scroller->isScrolling ())
    {
        scroller->stopScrolling ();
        delay = 0; 
        return FALSE;
    }
    
    // wait approx. 0.8 seconds before starting to scroll
    if (delay++ < 5) return TRUE;
    
    // move the view
    scroller->scroll ();
    
    return TRUE; 
}


// ctor
Scrollable::Scrollable()
{
    scrolling = false;
    scroll_offset = { 0, 0 };

    createScrollCursor();
}

// dtor
Scrollable::~Scrollable()
{
    gdk_cursor_unref (scrollCursor);
}

// prepare everything for 'auto-scrolling' (TM) ;-)
void Scrollable::prepareScrolling (const GdkPoint & point)
{
    if (!scrollingAllowed ()) return;
    
    int scroll_x = 0;
    int scroll_y = 0;
    
    GtkWidget *widget = drawingArea ();

    GtkAllocation allocation;
    gtk_widget_get_allocation (widget, &allocation);

    // set scrolling offset and direction    
    if (point.x < ACTIVE_BORDER) scroll_x = SCROLL_OFFSET;
    if (point.y < ACTIVE_BORDER) scroll_y = SCROLL_OFFSET;
    if (point.x + ACTIVE_BORDER > allocation.width) scroll_x = -SCROLL_OFFSET;
    if (point.y + ACTIVE_BORDER > allocation.height) scroll_y = -SCROLL_OFFSET;
    
    // enable scrolling
    if (scroll_x || scroll_y)
    {
        scroll_offset.x = scroll_x;
        scroll_offset.y = scroll_y;
        
        if (!scrolling)
        {
            GdkWindow *window = gtk_widget_get_window (widget);
            gdk_window_set_cursor (window, scrollCursor);

            scrolling = true;
            
            // make sure that drawing has priority over scrolling
            g_timeout_add_full (G_PRIORITY_DEFAULT_IDLE, 100, on_scroll_graph, this, NULL);
        }
    }
    else stopScrolling();
}

// scrolling or not?
bool Scrollable::isScrolling () const
{ 
    return scrolling; 
}

// stop scrolling
void Scrollable::stopScrolling ()       
{ 
    GdkWindow *window = gtk_widget_get_window (drawingArea());
    gdk_window_set_cursor (window, NULL);

    scrolling = false; 
}

// the cursor displayed while in the scrolling region
void Scrollable::createScrollCursor()
{
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data (SCROLL_CURSOR_pixel_data,
            GDK_COLORSPACE_RGB, TRUE, 8,
            SCROLL_CURSOR_SIZE, SCROLL_CURSOR_SIZE, SCROLL_CURSOR_SIZE * 4,
            NULL, NULL);
    scrollCursor = gdk_cursor_new_from_pixbuf (gdk_display_get_default(),
            pixbuf, SCROLL_CURSOR_SIZE / 2, SCROLL_CURSOR_SIZE / 2);
    g_object_unref (pixbuf);
}
