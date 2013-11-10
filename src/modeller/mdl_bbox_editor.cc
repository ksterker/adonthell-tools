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
 * @file modeller/mdl_bbox_editor.cc
 *
 * @author Kai Sterker
 * @brief Editing a models bbox.
 */

#include <adonthell/base/base.h>

#include "mdl_bbox_editor.h"

/// indices of text fields
enum
{
    EDIT_POS_X = 2,
    EDIT_POS_Y = 3,
    EDIT_POS_Z = 4,
    EDIT_SIZE_X = 5,
    EDIT_SIZE_Y = 6,
    EDIT_SIZE_Z = 7
};

// ctor
BboxEditor::BboxEditor ()
{
    Handles = new BboxHandles();
}

// dtor
BboxEditor::~BboxEditor()
{
    delete Handles;
}

// update shape size or position via mouse
bool BboxEditor::handleDragged (world::cube3 *shape, GdkPoint *dist, const bool & modifier) const
{
    // list of points that will get updated
    int points[world::cube3::NUM_CORNERS];
    // the offset by which the points will be moved
    world::vector3<s_int16> offset;

    switch (Handles->selected())
    {
        case POSITION:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;

            if (modifier)
            {
                offset.set_z (-dist->y);
            }
            else
            {
                offset.set_x (dist->x);
                offset.set_y (dist->y);
            }
            break;
        }
        case LENGTH:
        {
            // move the points on the cube's right side (x axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[4] = -1;

            offset.set_x (dist->x);
            break;
        }
        case WIDTH:
        {
            // move the points on the cube's back side (y axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_BACK_LEFT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = -1;

            offset.set_y (dist->y);
            break;
        }
        case HEIGHT:
        {
            // move the points on the cube's bottom side (z axis)
            points[0] = world::cube3::TOP_FRONT_LEFT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::TOP_BACK_RIGHT;
            points[3] = world::cube3::TOP_BACK_LEFT;
            points[4] = -1;

            offset.set_z (-dist->y);
            break;
        }
        default:
        {
            // no handle selected
            return false;
        }
    }

    // take care of scaling
    offset = offset * (1.0f / base::Scale);

    // handle small movements in scaled mode
    if (offset.squared_length() == 0) return false;

    // now update the selected points
    for (int i = 0; i < world::cube3::NUM_CORNERS; i++)
    {
        if (points[i] == -1) break;

        world::vector3<s_int16> curPos = shape->get_point (points[i]);
        shape->set_point (points[i], curPos + offset);
    }

    return true;
}

// update shape size or position via direct text entry
bool BboxEditor::updateShape (world::cube3 *shape, const int & pos, const int & value) const
{
    // list of points that will get updated
    int points[world::cube3::NUM_CORNERS];
    // the offset by which the points will be moved
    world::vector3<s_int16> offset;

    switch (pos)
    {
        case EDIT_POS_X:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;

            offset.set_x (value - shape->min_x());
            break;
        }
        case EDIT_POS_Y:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;

            offset.set_y (value - shape->min_y());
            break;
        }
        case EDIT_POS_Z:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;

            offset.set_z (value - shape->min_z());
            break;
        }
        case EDIT_SIZE_X:
        {
            // move the points on the cube's right side (x axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[4] = -1;

            offset.set_x (value - (shape->max_x() - shape->min_x()));
            break;
        }
        case EDIT_SIZE_Y:
        {
            // move the points on the cube's back side (y axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_BACK_LEFT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = -1;

            offset.set_y (value - (shape->max_y() - shape->min_y()));
            break;
        }
        case EDIT_SIZE_Z:
        {
            // move the points on the cube's bottom side (z axis)
            points[0] = world::cube3::TOP_FRONT_LEFT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::TOP_BACK_RIGHT;
            points[3] = world::cube3::TOP_BACK_LEFT;
            points[4] = -1;

            offset.set_z (value - (shape->max_z() - shape->min_z()));
            break;
        }
        default: return false;
    }

    // check if there is any modification at all
    if (offset.squared_length() == 0) return false;

    // now update the selected points
    for (int i = 0; i < world::cube3::NUM_CORNERS; i++)
    {
        if (points[i] == -1) break;

        world::vector3<s_int16> curPos = shape->get_point (points[i]);
        shape->set_point (points[i], curPos + offset);
    }

    return true;
}

// indicate which value will be affected by dragging handle
void BboxEditor::indicateEditingField (GtkEntry** shape_data, const s_int32 & handle, const bool & highlight) const
{
    int fields[] = { -1, -1, -1 };

    switch (handle)
    {
        case POSITION:
        {
            fields[0] = EDIT_POS_X;
            fields[1] = EDIT_POS_Y;
            fields[2] = EDIT_POS_Z; // with modifier key
            break;
        }
        case LENGTH:
        {
            fields[0] = EDIT_SIZE_X;
            break;
        }
        case WIDTH:
        {
            fields[0] = EDIT_SIZE_Y;
            break;
        }
        case HEIGHT:
        {
            fields[0] = EDIT_SIZE_Z;
            break;
        }
        default:
        {
            return;
        }
    }

    for (int i = 0; i < 3; i++)
    {
        if (fields[i] == -1) break;

        GtkStyle *style = NULL;
        if (highlight)
        {
            GdkColor color = { 0, 43760, 55745, 8315 };
            style = gtk_style_copy (gtk_widget_get_style (GTK_WIDGET (shape_data[fields[i]])));
            style->base[GTK_STATE_NORMAL]   = color;
            style->base[GTK_STATE_ACTIVE]   = color;
            style->base[GTK_STATE_PRELIGHT] = color;
            style->base[GTK_STATE_SELECTED] = color;
        }

        gtk_widget_set_style (GTK_WIDGET (shape_data[fields[i]]), style);
    }
}

// display size and position of current shape
void BboxEditor::updateShapeData (GtkEntry** shape_data, const world::cube3 *shape) const
{
    if (shape != NULL)
    {
        setShapeData (GTK_ENTRY(shape_data[EDIT_POS_X]), shape->min_x());
        setShapeData (GTK_ENTRY(shape_data[EDIT_POS_Y]), shape->min_y());
        setShapeData (GTK_ENTRY(shape_data[EDIT_POS_Z]), shape->min_z());
        setShapeData (GTK_ENTRY(shape_data[EDIT_SIZE_X]), shape->max_x() - shape->min_x());
        setShapeData (GTK_ENTRY(shape_data[EDIT_SIZE_Y]), shape->max_y() - shape->min_y());
        setShapeData (GTK_ENTRY(shape_data[EDIT_SIZE_Z]), shape->max_z() - shape->min_z());
    }
    else
    {
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POS_X]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POS_Y]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POS_Z]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_SIZE_X]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_SIZE_Y]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_SIZE_Z]), "");
    }
}
