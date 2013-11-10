/*
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com

 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file modeller/mdl_point_handles.cc
 *
 * @author Kai Sterker
 * @brief Handles to edit a models corner points.
 */


#include <adonthell/base/base.h>

#include "mdl_point_handles.h"

// ctor
PointHandles::PointHandles()
{
    Handles.resize(world::cube3::NUM_CORNERS);
}

// update position of handles used for manipulating shapes
void PointHandles::updateHandles (const world::cube3 *shape, const s_int16 & x, const s_int16 & y)
{
    for (int i = 0; i < world::cube3::NUM_CORNERS; i++)
    {
        world::vector3<s_int16> point = shape->get_point(i);

        Handles[i].x = x - HANDLE_OFFSET + point.x() * base::Scale;
        Handles[i].y = y - HANDLE_OFFSET + (point.y() - point.z()) * base::Scale;
    }
}
