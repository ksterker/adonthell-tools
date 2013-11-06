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
 * @file dlg_node_gfx.h
 *
 * @author Kai Sterker
 * @brief Base class for a DlgNode's graphical representation.
 */

#ifndef DLG_NODE_GFX_H
#define DLG_NODE_GFX_H

#include <gtk/gtkstyle.h>
#include "dlg_rect.h"
#include "dlg_types.h"

/**
 * This class provide the interface for the graphical representation of 
 * the node in the graph window. This includes functionality needed for
 * the actual rendering of the node, as well as positional information
 * (inherited from DlgRect).
 */
class DlgNodeGfx : public DlgRect
{
public:
    DlgNodeGfx () { }
    DlgNodeGfx (DlgPoint &position);
    virtual ~DlgNodeGfx () { }
    
    /** 
     * Change the mode of a node.
     * @param m the new mode.
     */
    void setMode (mode_type m)  { mode_ = m; }
    
    /** 
     * Get the current mode of a node.
     * @return the node's mode.
     */
    mode_type mode ()           { return mode_; }
    
    /**
     * Blit a certain area of the graph widget to the screen
     * @param widget the widget to update
     * @param area the rectangular area to update.
     */
    void update (GtkWidget *widget, DlgRect &area);
    
    /**
     * Draw this node to the given surface with the specified offset. 
     * The node's current mode determines the color thw node is drawn with. 
     * @param surface the cairo_surface_t to draw to
     * @param offset the DlgPoint to use as offset
     * @param widget the GtkWidget to update after drawing
     */
    virtual void draw (cairo_surface_t *surface, DlgPoint &offset, GtkWidget *widget) = 0;

    /**
     * Draw this node to the given surface. During drawing, the mode is
     * temporarily changed to the given mode.
     * @param surface the cairo_surface_t to draw to
     * @param offset the DlgPoint to use as offset
     * @param widget the GtkWidget to update after drawing
     * @param mode the new mode of the node
     */
    virtual void draw (cairo_surface_t *surface, DlgPoint &offset, GtkWidget *widget, mode_type mode) = 0;
       
    /**
     * Check whether the node is located at the given point.
     * @return <b>true</b> if this is the case, <b>false</b> otherwise.
     */
    virtual bool operator== (DlgPoint &point) { return contains (point); }
    
protected:
    mode_type mode_;        // This nodes mode (NONE, HILIGHTED, SELECTED)
};

#endif // DLG_NODE_GFX_H
