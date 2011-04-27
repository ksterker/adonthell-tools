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
 * @file modeller/mdl_bbox_handles.h
 *
 * @author Kai Sterker
 * @brief Handles to edit a models bbox.
 */

#ifndef MDL_BBOX_HANDLES_H_
#define MDL_BBOX_HANDLES_H_

#include "mdl_handle.h"

/**
 * Handle types
 */
enum
{
    POSITION = 0,
    LENGTH = 1,
    WIDTH = 2,
    HEIGHT = 3,
    MAX_HANDLES = 4
};

/**
 * Handles for editing the bounding box of a shape.
 */
class BboxHandles : public Handles
{
public:
    /**
     * Initialize list of handles.
     */
    BboxHandles();

    /**
     * Update position of the edit handles for the active shape.
     * @param shape shape for which to set the handles.
     * @param x x-offset of rendered model
     * @param y y-offset of rendered model
     */
    virtual void updateHandles(const world::cube3 *shape, const s_int16 & x, const s_int16 & y);
};

#endif /* MDL_BBOX_HANDLES_H_ */
