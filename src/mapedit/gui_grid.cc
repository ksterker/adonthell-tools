/*
 $Id: gui_grid.cc,v 1.1 2009/05/24 13:40:06 ksterker Exp $
 
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
 * @file mapedit/gui_grid.cc
 *
 * @author Kai Sterker
 * @brief Grid for aligning object placement.
 */

#include "gui_grid.h"

// ctor
GuiGrid::GuiGrid (gfx::surface *overlay)
{
    Overlay = overlay;
    Changed = false;
    Visible = false;
    SnapToGrid = true;
    Ox = 0;
    Oy = 0;
    Ix = 64;
    Iy = 48;
}

// draw the grid
void GuiGrid::draw (const s_int32 & x, const s_int32 & y, const u_int16 & l, const u_int16 & h)
{
    if (Changed)
    {
        // clear previous grid
        Overlay->fillrect (x, y, l, h, 0);
        Changed = false;
    }
    
    if (Visible)
    {
        int l = Overlay->length();
        int h = Overlay->height();
        
        // draw vertical lines
        for (int i = Ox; i < l; i += Ix)
        {
            Overlay->fillrect (i, 0, 1, h, 0x88FFFFFF);
        }
        
        // draw horizontal lines
        for (int j = Oy; j < h; j += Iy)
        {
            Overlay->fillrect (0, j, l, 1, 0x88FFFFFF);
        }
    }
}

// set the grid from the given object
void GuiGrid::grid_from_object (world::chunk_info & ci, const s_int32 & ox, const s_int32 & oy)
{
    if (ci.get_object()->length() && ci.get_object()->width())
    {
        // get location
        s_int32 x = ci.Min.x() - ox;
        s_int32 y = ci.Min.y() - oy;
        
        // get extend --> that will be our interval
        Ix = ci.get_object()->length();
        Iy = ci.get_object()->width();
        
        // set offset from object position and map view offset
        Ox = x < 0 ? x % Ix + Ix : x % Ix;
        Oy = y < 0 ? y % Iy + Iy : y % Iy - ci.get_object()->cur_y();

        // make sure the changes take effect on next redraw
        Changed = true;
    }
}

// align to grid
world::vector3<s_int32> GuiGrid::align_to_grid (const world::vector3<s_int32> & pos)
{
    if (SnapToGrid)
    {
        // calculate offset from grid
        s_int32 x = (pos.x() - Ox) % Ix;
        s_int32 y = (pos.y() - Oy) % Iy;

        // move to grid
        world::vector3<s_int32> result (pos.x() - x, pos.y() - y, pos.z());
        return result;
    }
    
    return pos;
}

// move the grid
void GuiGrid::scroll (const s_int16 & x, const s_int16 & y)
{
    Ox += x;
    while (Ox < 0) Ox += Ix;
    while (Ox > Ix) Ox -= Ix;
    
    Oy += y;
    while (Oy < 0) Oy += Iy;
    while (Oy > Iy) Oy -= Iy;
    
    Changed = true;
    draw (); 
}

// toggle grid on/off
void GuiGrid::set_visible (const bool & visible)
{
    if (visible != Visible)
    {
        Visible = visible;
        Changed = true;
        draw ();
    }
}
