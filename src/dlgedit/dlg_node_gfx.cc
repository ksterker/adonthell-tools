/*
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
 * @file dlg_node_gfx.cc
 *
 * @author Kai Sterker
 * @brief Base class for a DlgNode's graphical representation.
 */

#include <gtk/gtk.h>
#include "dlg_node_gfx.h"

// blit part of widget to the screen
void DlgNodeGfx::update (GtkWidget *widget, DlgRect &area)
{
    if (widget)
    {
        GdkRectangle rect = (GdkRectangle) area;
        gdk_window_invalidate_rect (gtk_widget_get_window(widget), &rect, FALSE);
    }
}
