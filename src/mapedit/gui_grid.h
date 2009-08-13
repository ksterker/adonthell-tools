/*
 $Id: gui_grid.h,v 1.1 2009/05/24 13:40:17 ksterker Exp $
 
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
 * @file mapedit/gui_grid.h
 *
 * @author Kai Sterker
 * @brief Grid for aligning object placement.
 */

#ifndef GUI_GRID_H
#define GUI_GRID_H

#include <gfx/surface.h>
#include <world/chunk_info.h>

class GuiGrid
{
public:
    /**
     * Create the grid.
     */
    GuiGrid (gfx::surface *overlay);
    
    /**
     * (Re)draw the entire grid.
     */
    void draw ()
    {
        draw (0, 0, Overlay->length(), Overlay->height());
    }
    
    /**
     * Draw grid in the given area
     * @param x starting point of the area.
     * @param y starting point of the area.
     * @param l the length of the area
     * @param h the height of the area
     */
    void draw (const s_int32 & x, const s_int32 & y, const u_int16 & l, const u_int16 & h);
    
    /**
     * Move the grid.
     * @param x offset to scroll grid in x direction
     * @param y offset to scroll grid in y direction
     */
    void scroll (const s_int16 & x, const s_int16 & y);

    /**
     * Update the grid from the size of the given object.
     * @param ci an object already placed on the map.
     * @param ox x-offset of map view 
     * @param oy y-offset of map view
     */
    void grid_from_object (world::chunk_info & ci, const s_int32 & ox, const s_int32 & sy);

    /**
     * Align the given position to the grid.
     * @param return the position aligned to the grid.
     */
    world::vector3<s_int32> align_to_grid (const world::vector3<s_int32> & pos);

    /**
     * Toggle grid on or off
     * @param visible true to show the grid, false to hide it.
     */
    void set_visible (const bool & visible);
    
    /**
     * Toggle snapping on or off. Decides whether objects will be
     * automatically aligned to the current grid.
     * @param snap true to turn snapping on, false to turn it off.
     */
    void set_snap_to_grid (const bool & snap)
    {
        SnapToGrid = snap;
    }
    
protected:
    /// x offset for grid
    s_int16 Ox;
    /// y offset for grid
    s_int16 Oy;
    /// x interval of grid
    u_int16 Ix;
    /// y interval of grid
    u_int16 Iy;
    
private:
    /// overlay onto which to draw grid
    gfx::surface *Overlay;
    /// whether grid needs to be rendered
    bool Visible;
    /// whether to align objects with grid
    bool SnapToGrid;
    /// whether the grid has changed
    bool Changed;
};

#endif // GUI_GRID
