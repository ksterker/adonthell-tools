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
 * @file modeller/mdl_bbox_handles.cc
 *
 * @author Kai Sterker
 * @brief Handles to edit a models bbox.
 */


#include <base/base.h>

#include "mdl_bbox_handles.h"

// ctor
BboxHandles::BboxHandles()
{
    Handles.resize(MAX_HANDLES);
}

// update position of handles used for manipulating shapes
void BboxHandles::updateHandles (const world::cube3 *shape, const s_int16 & x, const s_int16 & y)
{
    s_int16 length = (shape->max_x() - shape->min_x()) * base::Scale;
    s_int16 width = (shape->max_y() - shape->min_y()) * base::Scale;
    s_int16 height = (shape->max_z() - shape->min_z()) * base::Scale;

    // move position (x/y/z) handle in the top north-west corner of the shape
    Handles[POSITION].x = x - HANDLE_OFFSET;
    Handles[POSITION].y = y - HANDLE_OFFSET - height;

    // change length handle in the top east vector of the shape
    Handles[LENGTH].x = x - HANDLE_OFFSET + length;
    Handles[LENGTH].y = y - HANDLE_OFFSET - height + width / 2;

    // change width handle in the top south vector of the shape
    Handles[WIDTH].x = x - HANDLE_OFFSET + length / 2;
    Handles[WIDTH].y = y - HANDLE_OFFSET - height + width;

    // change height handle in the top north vector of the shape
    Handles[HEIGHT].x = x - HANDLE_OFFSET + length / 2;
    Handles[HEIGHT].y = y - HANDLE_OFFSET - height;
}
