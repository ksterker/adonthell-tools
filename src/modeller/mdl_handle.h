/*
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file modeller/mdl_handle.h
 *
 * @author Kai Sterker
 * @brief Definitions for shape handles.
 */

#ifndef MDL_HANDLE_H
#define MDL_HANDLE_H

#include <vector>
#include <gdk/gdk.h>
#include <world/cube3.h>

/// size of handle
#define HANDLE_SIZE 8
/// offset from handle center
#define HANDLE_OFFSET HANDLE_SIZE/2

/**
 * Base class for Handles to editing shapes in a
 * specific way using the mouse. A single handle
 * represents a point that can be dragged to alter
 * shape size, position, etc.
 */
class Handles
{
public:
    /**
     * Update handle position relative to where the shape is rendered.
     */
    virtual void updateHandles(const world::cube3 *shape, const s_int16 & x, const s_int16 & y) = 0;

    /**
     * Get number of handles.
     * @return maximum number of handles.
     */
    u_int32 size () const
    {
        return Handles.size();
    }

    /**
     * Get handle at given index.
     * @param index the index of the handle
     * @return the handle at given index.
     */
    GdkPoint operator[] (const u_int32 & index) const
    {
        return Handles[index];
    }

protected:
    /// list of handle positions
    std::vector<GdkPoint> Handles;
};


#endif
