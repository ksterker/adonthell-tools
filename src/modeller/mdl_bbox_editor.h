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
 * @file modeller/mdl_bbox_editor.h
 *
 * @author Kai Sterker
 * @brief Editing a models bbox.
 */

#ifndef MDL_BBOX_MODE_H
#define MDL_BBOX_MODE_H

#include <gtk/gtk.h>

#include <world/cube3.h>

/**
 * Editor for modifying the bounding box of a shape,
 * i.e. its position, length, width and height.
 */
class BboxEditor
{
public:
    /**
     * Update the given shape by moving the handle by the given distance.
     * @param shape the shape being edited.
     * @param handle index of the handle being dragged.
     * @param dist distance the handle has been dragged.
     * @param modifier whether the modifier key (shift) was pressed while dragging.
     */
    virtual bool handleDragged (world::cube3 *shape, const int & handle, GdkPoint *dist, const bool & modifier) const;

    /**
     * Update the given shape at the given position with the given value.
     * @param shape the shape being edited.
     * @param pos position of the shape being edited.
     * @param value new value for that position.
     */
    virtual bool updateShape (world::cube3 *shape, const int & pos, const int & value) const;

    /**
     * Indicate which value will be affected by dragging a handle.
     * @param shape_data the list of editing fields.
     * @param handle index of the handle being dragged.
     * @param highlight true to turn indicator on, false to turn indicator off.
     */
    virtual void indicateEditingField (GtkEntry** shape_data, const s_int32 & handle, const bool & highlight) const;

    /**
     * Display size and position of given shape.
     * @param shape_data the list of editing fields.
     * @param shape the shape of which to set the data.
     */
    virtual void updateShapeData (GtkEntry** shape_data, const world::cube3 *shape) const;

    /**
     * Fill in one piece of data of the given shape.
     * @param entry the entry to update.
     * @param value the new value to display.
     */
    void setShapeData (GtkEntry *entry, const s_int32 & value) const;
};

#endif /* MDL_BBOX_MODE_H */
