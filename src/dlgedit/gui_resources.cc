/*
   $Id: gui_resources.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file gui_resources.cc
 *
 * @author Kai Sterker
 * @brief Provides fonts and Colors used throughout dlgedit.
 */

#include <gtk/gtk.h>
#include "gui_resources.h"

/**
 * The font for text output.
 */
PangoLayout *GuiResources::Font;

/**
 * Some pens for line drawing.
 */
GdkGC *GuiResources::Color[MAX_GC];

// create font and pens
void GuiResources::init (GtkWidget *widget)
{
    // font to use on the drawing area
    Font = gtk_widget_create_pango_layout (widget, NULL);
    
    GdkColor c;

    GdkWindow     *window = gtk_widget_get_parent_window (widget);
    GdkColormap *colormap = gtk_widget_get_colormap (widget);
  
    // Black
    Color[GC_BLACK] = gdk_gc_new (window);
    c.red   = 0;
    c.green = 0;
    c.blue  = 0;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_BLACK], &c);
    
    // White
    Color[GC_WHITE] = gdk_gc_new (window);
    c.red   = 65535;
    c.green = 65535;
    c.blue  = 65535;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_WHITE], &c);

    // Grey (for NPC node background)
    Color[GC_GREY] = gdk_gc_new (window);
    c.red   = 55000;
    c.green = 55000;
    c.blue  = 55000;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_GREY], &c);

    // Dark Red (for selected nodes)
    Color[GC_DARK_RED] = gdk_gc_new (window);
    c.red   = 65535;
    c.green = 0;
    c.blue  = 0;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_DARK_RED], &c);

    // Red
    Color[GC_RED] = gdk_gc_new (window);
    c.red   = 65535;
    c.green = 16350;
    c.blue  = 0;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_RED], &c);

    // Green (for narrator node background)
    Color[GC_GREEN] = gdk_gc_new (window);
    c.red   = 51117;
    c.green = 65355;
    c.blue  = 48496;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_GREEN], &c);

    // Dark Green (for narrator nodes)
    Color[GC_DARK_GREEN] = gdk_gc_new (window);
    c.red   = 0;
    c.green = 27300;
    c.blue  = 15600;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_DARK_GREEN], &c);

    // Orange (for highlighting nodes)
    Color[GC_ORANGE] = gdk_gc_new (window);
    c.red   = 62258;
    c.green = 43253;
    c.blue  = 9175;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_ORANGE], &c);

    // Dark Blue (for player nodes)
    Color[GC_DARK_BLUE] = gdk_gc_new (window);
    c.red   = 0;
    c.green = 0;
    c.blue  = 60000;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_DARK_BLUE], &c);

    // Blue - like the sky (for player node background)
    Color[GC_BLUE] = gdk_gc_new (window);
    c.red   = 48496;
    c.green = 50461;
    c.blue  = 65535;
    gdk_colormap_alloc_color (colormap, &c, TRUE, TRUE);
    gdk_gc_set_foreground (Color[GC_BLUE], &c);

}

// Get a certain predefined Color
GdkGC *GuiResources::getColor (int color)
{
    if (color < 0 || color > MAX_GC) return (GdkGC *) NULL;
    else return Color[color];
}

// Get the right Color for a (mode, type) combination
GdkGC *GuiResources::getColor (mode_type mode, node_type type)
{    
    switch (mode)
    {
        // not selected    
        case IDLE:
        {
            if (type == NPC || type == LINK || type == MODULE) 
                                       return Color[GC_BLACK];
            else if (type == NARRATOR) return Color[GC_DARK_GREEN];
            else if (type == PLAYER)   return Color[GC_DARK_BLUE];
            else                       return Color[GC_BLACK];
        }

        // selected
        case NODE_SELECTED:
        {
            return Color[GC_DARK_RED];
        }

        // highlighted
        case NODE_HILIGHTED:
        {
            return Color[GC_ORANGE];
        }

        // unknown
        default:
            return Color[GC_BLACK];
    }

    return (GdkGC *) NULL;
}

// Get the right Fill Color for a (mode, type) combination
// This color is going to be filled inside the node cirle
GdkGC *GuiResources::getFill (mode_type mode, node_type type)
{    
    switch (type)
    {
        case PLAYER:
            return Color[GC_BLUE];
        case NPC:
            return Color[GC_GREY];
        case NARRATOR:
            return Color[GC_GREEN];
        // unknown
        default:
            return Color[GC_WHITE];
    }

    return (GdkGC *) NULL;
}
