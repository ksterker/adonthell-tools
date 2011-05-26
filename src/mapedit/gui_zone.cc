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

#include <base/base.h>
#include <gfx/gfx.h>

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
        
        // set clipping rectangle
        gfx::drawing_area da (x, y, l, h);

        std::list<world::zone*> zones = map->zones_in_view (x / base::Scale + map->x(), y / base::Scale + map->y(), map->z(), l, h);
        for (std::list<world::zone*>::const_iterator i = zones.begin(); i != zones.end(); i++)
        {
            u_int8 blue = *i == ActiveZone ? 0xFF : 0x00;

            // get zone location
            s_int16 sx = ((*i)->min().x() - map->x()) * base::Scale;
            s_int16 sy = ((*i)->min().y() - (*i)->min().z() - map->y() + map->z()) * base::Scale;
            
            // get zone extend
            s_int16 ex = ((*i)->max().x() - (*i)->min().x()) * base::Scale;
            s_int16 ey = ((*i)->max().y() - (*i)->min().y()) * base::Scale;
            s_int16 ez = ((*i)->max().z() - (*i)->min().z()) * base::Scale;

            gfx::surface *area = gfx::create_surface();
            area->set_alpha (48);
            area->resize (ex, ey);
            area->fillrect(0, 0, ex, ey, area->map_color (0x88, 0xFF, blue));

            // draw zone bottom area
            area->draw (sx, sy, &da, Overlay);

            // draw zone top area
            area->draw (sx, sy - ez, &da, Overlay);

            u_int32 col = Overlay->map_color (0x88, 0xFF, blue, 0xBB);

            // draw bottom edges
            Overlay->draw_line(sx, sy, sx, sy + ey, col, &da);
            Overlay->draw_line(sx, sy, sx + ex, sy, col, &da);
            Overlay->draw_line(sx + ex, sy, sx + ex, sy + ey, col, &da);
            Overlay->draw_line(sx, sy + ey, sx + ex, sy + ey, col, &da);

            // draw top edges
            Overlay->draw_line(sx, sy - ez, sx, sy - ez + ey, col, &da);
            Overlay->draw_line(sx, sy - ez, sx + ex, sy - ez, col, &da);
            Overlay->draw_line(sx + ex, sy - ez, sx + ex, sy - ez + ey, col, &da);
            Overlay->draw_line(sx, sy - ez + ey, sx + ex, sy - ez + ey, col, &da);

            // draw edges between top and bottom
            Overlay->draw_line(sx, sy, sx, sy - ez, col, &da);
            Overlay->draw_line(sx, sy + ey, sx, sy + ey - ez, col, &da);
            Overlay->draw_line(sx + ex, sy, sx + ex, sy - ez, col, &da);
            Overlay->draw_line(sx + ex, sy + ey, sx + ex, sy + ey - ez, col, &da);

            // cleanup
            delete area;
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

// update zone display
void GuiZone::update ()
{
    if (Visible)
    {
        draw (0, 0, Overlay->length(), Overlay->height());
    }
}
