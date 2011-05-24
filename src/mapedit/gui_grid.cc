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
 * @file mapedit/gui_grid.cc
 *
 * @author Kai Sterker
 * @brief Grid for aligning object placement.
 */

#include "base/base.h"
#include "gui_grid.h"

// ctor
GuiGrid::GuiGrid (gfx::surface *overlay)
{
    Overlay = overlay;
    Changed = false;
    Visible = false;
    SnapToGrid = true;
    AutoAdjust = true;
    PreventOverlap = true;
    
    CurObject = NULL;
    Monitor = NULL;
    
    Alignment = ALIGN_TOP | ALIGN_LEFT;
    
    Ox = 0;
    Oy = 0;
    Mx = 0;
    My = 0;
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
        for (int i = Mx * base::Scale; i < l; i += Ix * base::Scale)
        {
            Overlay->fillrect (i, 0, 1, h, 0x88FFFFFF);
        }
        
        // draw horizontal lines
        for (int j = My * base::Scale; j < h; j += Iy * base::Scale)
        {
            Overlay->fillrect (0, j, l, 1, 0x88FFFFFF);
        }
    }
}

// set the grid from the given object
void GuiGrid::grid_from_object (world::chunk_info & ci, const s_int32 & ox, const s_int32 & oy)
{
    delete CurObject;
    CurObject = new world::chunk_info (ci.get_entity(), ci.Min, ci.Max);
    
    if (Monitor)
    {
        Monitor->objChanged();
    }

    if (AutoAdjust)
    {
        grid_from_cur_object (ox, oy);
    }
}

// set the grid from the given object
void GuiGrid::grid_from_cur_object (const s_int32 & ox, const s_int32 & oy)
{
    if (CurObject == NULL) return;

    if (CurObject->get_object()->length() && CurObject->get_object()->width())
    {
        // get location
        s_int32 x = CurObject->Min.x() - ox;
        s_int32 y = CurObject->Min.y() - oy;
        
        // get extend --> that will be our interval
        Ix = CurObject->get_object()->length();
        Iy = CurObject->get_object()->width();

        // set offset from object position
        Ox = CurObject->Min.x() % Ix;
        Oy = CurObject->Min.y() % Iy;

        // store map view offset
        Mx = x < 0 ? x % Ix + Ix : x % Ix;
        My = y < 0 ? y % Iy + Iy : y % Iy - CurObject->get_object()->cur_y();
        
        // make sure the changes take effect on next redraw
        Changed = true;
        
        // display info about changed grid
        if (Monitor)
        {
            Monitor->gridChanged();
        }
    }
}

// align to grid
world::vector3<s_int32> GuiGrid::align_to_grid (const world::vector3<s_int32> & pos)
{
    if (SnapToGrid)
    {
        // calculate offset from grid
        s_int32 x = (pos.x() - Mx) % Ix;
        s_int32 y = (pos.y() - My) % Iy;
        
        // TODO: the following produces good results if the grid is larger
        //       than the object being placed. If it's the other way round,
        //       the object aligns correctly with the grid, but no longer
        //       in the cell at the position of the mouse pointer.
        
        if ((Alignment & ALIGN_LEFT) == ALIGN_LEFT)
        {
            // align left
        }
        else if ((Alignment & ALIGN_RIGHT) == ALIGN_RIGHT)
        {
            // align right
            x -= Ix - CurObject->get_object()->length();
        }
        else
        {
            // center
            x += (CurObject->get_object()->length() - Ix) / 2;
        }
        
        if ((Alignment & ALIGN_TOP) == ALIGN_TOP)
        {
             // align top
        }
        else if ((Alignment & ALIGN_BOTTOM) == ALIGN_BOTTOM)
        {
            // align bottom
            y -= Iy - CurObject->get_object()->width();
        }
        else
        {
            // center
            y += (CurObject->get_object()->width() - Iy) / 2;
        }        

        // move to grid
        world::vector3<s_int32> result (pos.x() - x, pos.y() - y, pos.z());
        return result;
    }
    
    return pos;
}

// move the grid
void GuiGrid::scroll (const s_int16 & x, const s_int16 & y)
{
    Mx += x;
    while (Mx < 0) Mx += Ix;
    while (Mx > Ix) Mx -= Ix;
    
    My += y;
    while (My < 0) My += Iy;
    while (My > Iy) My -= Iy;
    
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
