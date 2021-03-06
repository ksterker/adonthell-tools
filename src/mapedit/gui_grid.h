/*
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

#include <adonthell/gfx/surface.h>
#include <adonthell/world/chunk_info.h>

class MapEntity;

/**
 * Interface for classes interested in monitoring
 * changes to the grid.
 */
class GridMonitor
{
public:
    /**
     * Destructor.
     */
    virtual ~GridMonitor() {}
    
    /**
     * Notify that the grid has changed.
     */
    virtual void gridChanged() = 0;

    /**
     * Notify that the auto-adjust object has changed.
     */
    virtual void objChanged() = 0;
};

/**
 * A grid to which object placed on the map can be
 * aligned to.
 */
class GuiGrid
{
public:
    /** Directions for aligning the grid. */
    enum
    {
        ALIGN_LEFT = 1,
        ALIGN_RIGHT = 2,
        ALIGN_TOP = 4,
        ALIGN_BOTTOM = 8
    };
    
    /**
     * Create the grid.
     */
    GuiGrid (gfx::surface *overlay);
    
    /**
     * Destroy the grid
     */
    ~GuiGrid ();

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
     * @param entity an object selected for placing on the map.
     * @param ox x-offset of map view 
     * @param oy y-offset of map view
     */
    void grid_from_object (MapEntity *entity, const s_int32 & ox, const s_int32 & oy);

    /**
     * Update the grid from the size of the object currently 
     * used for painting.
     * @param ox x-offset of map view 
     * @param oy y-offset of map view
     */
    void grid_from_cur_object (const s_int32 & ox, const s_int32 & oy);

    /**
     * Adjust the grid based on connector data.
     */
    void adjust_to_connectors ();

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
     * Set reference object to adjust the grid to
     * @param pos position of the entity on the map.
     * @param entity the object acting as reference.
     */
    void set_reference (const world::vector3<s_int32> & pos, MapEntity *entity);

    /**
     * Returns whether it is forbidden to place overlapping objects.
     * @return true if overlapping forbidden, false otherwise.
     */
    bool overlap_prevented () const
    {
        return PreventOverlap;
    }
    
    /// can edit the grid
    friend class GuiGridDialog;
    
protected:
    /// x offset for grid (object position)
    s_int16 Ox;
    /// y offset for grid (object position)
    s_int16 Oy;
    /// x offset for grid (map view)
    s_int16 Mx;
    /// y offset for grid (map view)
    s_int16 My;
    /// x interval of grid
    u_int16 Ix;
    /// y interval of grid
    u_int16 Iy;

    /// how objects get aligned to the grid
    s_int32 Alignment;

    /// notification of grid changes
    GridMonitor *Monitor;

private:
    /// object selected for painting the map
    MapEntity *CurObject;
    /// object used to align the grid to
    MapEntity *RefObject;
    /// temporary entity for RefObjects not present on map
    world::entity *RefEntity;
    /// position of reference object on the map
    world::chunk_info *RefLocation;
    /// overlay onto which to draw grid
    gfx::surface *Overlay;
    /// whether grid needs to be rendered
    bool Visible;
    /// whether to align objects with grid
    bool SnapToGrid;
    /// whether grid automatically adjusts to selected object
    bool AutoAdjust;
    /// whether overlapping objects are forbidden
    bool PreventOverlap;
    /// whether the grid has changed
    bool Changed;
};

#endif // GUI_GRID
