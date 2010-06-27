/*
 Copyright (C) 2010 Kai Sterker <kaisterker@linuxgames.com>
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
* @file mapedit/gui_zone.h
*
* @author Kai Sterker
* @brief Display zones on the mapview.
*/

#ifndef GUI_ZONE_H
#define GUI_ZONE_H

#include <gfx/surface.h>

/**
 * Render zones on the mapview.
 */
class GuiZone
{
public:
    /**
     * Create the zone view.
     */
    GuiZone (gfx::surface *overlay);
    
    /**
     * Draw zones in the given area
     * @param x starting point of the area.
     * @param y starting point of the area.
     * @param l the length of the area
     * @param h the height of the area
     */
    void draw (const s_int32 & x, const s_int32 & y, const u_int16 & l, const u_int16 & h);
    
    /**
     * Set zones visible.
     * @param visible true to show zones.
     */
    void set_visible (const bool & visible);

private:
    /// overlay onto which to draw grid
    gfx::surface *Overlay;

    /// whether zones should be rendered
    bool Visible;
    /// whether re-rendering is required
    bool Changed;
};

#endif
