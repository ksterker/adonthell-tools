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
 * @file mapedit/gui_zone.cc
 *
 * @author Kai Sterker
 * @brief Grid for aligning object placement.
 */

#include "gui_zone.h"
#include "map_data.h"
#include "map_mgr.h"

// ctor
GuiZone::GuiZone (gfx::surface *overlay)
{
    Overlay = overlay;
    Changed = false;
    Visible = false;
}

// render zones
void GuiZone::draw (const s_int32 & x, const s_int32 & y, const u_int16 & l, const u_int16 & h)
{
    if (Changed)
    {
        Overlay->fillrect (x, y, l, h, 0);
        Changed = false;
    }
    
    if (Visible)
    {
        MapData *map = (MapData *) MapMgr::get_map ();
        
        std::list<world::zone*> zones = map->zones_in_view (x + map->x(), y + map->y(), map->z(), l, h);
        for (std::list<world::zone*>::const_iterator i = zones.begin(); i != zones.end(); i++)
        {
            // get zone location
            s_int16 sx = (*i)->min().x() - map->x() - x;
            s_int16 sy = (*i)->min().y() - (*i)->min().z() - map->y() + map->z() - y;
            
            // get zone extend
            s_int16 ex = (*i)->max().x() - (*i)->min().x();
            s_int16 ey = (*i)->max().y() - (*i)->min().y();
            
            // draw zone with 5px border
            for (int b = 0; b < 6; b++)
            {
                u_int32 col = Overlay->map_color (0x88, 0xFF, 0x00, 0xFF - 40 * b);
                Overlay->fillrect (sx + b, sy + b, ex - 2*b, ey - 2*b, col);
            }
        }
    }
}

// toggle zones on/off
void GuiZone::set_visible (const bool & visible)
{
    if (visible != Visible)
    {
        Visible = visible;
        Changed = true;
    }
}
