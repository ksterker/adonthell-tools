/*
   $Id: gui_resources.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file gui_resources.h
 *
 * @author Kai Sterker
 * @brief Provides fonts and colors used throughout dlgedit.
 */

#ifndef GUI_RESOURCES_H
#define GUI_RESOURCES_H

#include <gtk/gtkstyle.h>
#include "dlg_types.h"

/**
 * This class provides some fonts and pens that are used throughout
 * dlgedit. Instead of creating the newly each time something needs
 * to be drawn, they are created once when the program starts.
 */
class GuiResources
{
public:
    /**
     * Create all the fonts and colors needed.
     * @param widget the widget we later draw to 
     */
    static void init (GtkWidget *widget);

    /**
     * Retrieve a colored 'pen' to draw onto a drawing area.
     * @param color the color to retrieve
     * @return a GdkGC.
     */
    static GdkGC *getColor (int color);
    /**
     * Retrieve a colored 'pen' to draw onto a drawing area, depending
     * on the given mode and type.
     * @param mode the mode of a DlgNode
     * @param type the type of a DlgNode
     * @return a GdkGC.
     */
    static GdkGC *getColor (mode_type mode, node_type type);
    /**
     * Retrieve a color to fill drawn objects on a drawing area, 
     * depending on the given mode and type.
     * @param mode the mode of a DlgNode
     * @param type the type of a DlgNode
     * @return a GdkGC.
     */
    static GdkGC *getFill (mode_type mode, node_type type);
    /**
     * Retrieve the font used to draw text onto a drawing area.
     * @return a GdkFont.
     */
    static GdkFont *font ()         { return Font; }

private:
    static GdkFont *Font;           // font for text-output
    static GdkGC *Color[MAX_GC];    // custom Pens    
};

#endif // GUI_RESOURCES_H
