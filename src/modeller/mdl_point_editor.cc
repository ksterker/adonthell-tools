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
 * @file modeller/mdl_point_editor.cc
 *
 * @author Kai Sterker
 * @brief Editing a models individual points.
 */

#include <adonthell/base/base.h>

#include "mdl_point_editor.h"

/// indices of text fields
enum
{
    EDIT_POS_X = 2,
    EDIT_POS_Y = 3,
    EDIT_POS_Z = 4,
    EDIT_POINT_X = 5,
    EDIT_POINT_Y = 6,
    EDIT_POINT_Z = 7
};

// ctor
PointEditor::PointEditor ()
{
    Handles = new PointHandles();
}

// dtor
PointEditor::~PointEditor()
{
    delete Handles;
}

// update shape size or position via mouse
bool PointEditor::handleDragged (world::cube3 *shape, GdkPoint *dist, const bool & modifier) const
{
    // currently selected handle
    s_int32 handle = Handles->selected();
    // the offset by which the points will be moved
    world::vector3<s_int16> offset;
    // the point currently edited
    world::vector3<s_int16> point = shape->get_point (handle);

    if (modifier)
    {
        offset.set_z (-dist->y);
    }
    else
    {
        offset.set_x (dist->x);
        offset.set_y (dist->y);
    }

    // take care of scaling
    offset = offset * (1.0f / base::Scale);

    // handle small movements in scaled mode
    if (offset.squared_length() == 0) return false;

    // now update the selected point
    shape->set_point (handle, point + offset);

    return true;
}

// update shape size or position via direct text entry
bool PointEditor::updateShape (world::cube3 *shape, const int & pos, const int & value) const
{
    // no handle selected at all
    if (Handles->selected() == -1) return false;

    // the offset by which the points will be moved
    world::vector3<s_int16> offset;
    // the point currently edited
    world::vector3<s_int16> point = shape->get_point (Handles->selected());

    switch (pos)
    {
        case EDIT_POS_X:
        {
            offset.set_x (value - point.x());
            break;
        }
        case EDIT_POS_Y:
        {
            offset.set_y (value - point.y());
            break;
        }
        case EDIT_POS_Z:
        {
            offset.set_z (value - point.z());
            break;
        }
        default: return false;
    }

    // check if there is any modification at all
    if (offset.squared_length() == 0) return false;

    // now update the selected point
    shape->set_point (Handles->selected(), point + offset);

    return true;
}

// indicate which value will be affected by dragging handle
void PointEditor::indicateEditingField (GtkEntry** shape_data, const s_int32 & handle, const bool & highlight) const
{
    int fields[] = { EDIT_POS_X, EDIT_POS_Y, EDIT_POS_Z };

    // set handle position
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
void PointEditor::updateShapeData (GtkEntry** shape_data, const world::cube3 *shape) const
{
    s_int32 handle = Handles->selected();
    if (shape != NULL && handle != -1)
    {
        world::vector3<s_int16> point = shape->get_point(handle);

        setShapeData(GTK_ENTRY(shape_data[EDIT_POS_X]), point.x());
        setShapeData(GTK_ENTRY(shape_data[EDIT_POS_Y]), point.y());
        setShapeData(GTK_ENTRY(shape_data[EDIT_POS_Z]), point.z());

        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POINT_X]), (handle & 1) == 1 ? "Right" : "Left");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POINT_Y]), (handle & 2) == 2 ? "Front" : "Back");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POINT_Z]), (handle & 4) == 4 ? "Top" : "Bottom");
    }
    else
    {
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POS_X]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POS_Y]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POS_Z]), "");

        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POINT_X]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POINT_Y]), "");
        gtk_entry_set_text (GTK_ENTRY(shape_data[EDIT_POINT_Z]), "");
    }
}
