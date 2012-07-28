/*
 $Id: gui_mapview.cc,v 1.8 2009/05/24 13:40:28 ksterker Exp $
 
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
 * @file mapedit/map_mgr.h
 *
 * @author Kai Sterker
 * @brief Area manager extension for map editor.
 */

#include <adonthell/world/area_manager.h>

/**
 * Area manager that allows to directly set the 
 * active map.
 */
class MapMgr : public world::area_manager
{
public:
    /**
     * Set active map to the given instance.
     * @param map the new active map.
     */
    static void set_map (MapData *map)
    {
        // FIXME: pathfinding not updated yet
        ActiveMap = map;
    }
};
