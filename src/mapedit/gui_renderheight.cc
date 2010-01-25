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
 * @file mapedit/gui_renderheight.cc
 *
 * @author Kai Sterker
 * @brief Limit for rendering the map.
 */

#include "gui_renderheight.h"

// ctor
GuiRenderHeight::GuiRenderHeight()
{
    Range = gtk_vscale_new_with_range (0, 100, 10);
    gtk_scale_set_digits (GTK_SCALE(Range), 0);
    gtk_scale_set_draw_value (GTK_SCALE(Range), TRUE);
    gtk_scale_set_value_pos (GTK_SCALE(Range), GTK_POS_TOP);
    gtk_range_set_update_policy (GTK_RANGE(Range), GTK_UPDATE_DELAYED);
    gtk_range_set_inverted (GTK_RANGE(Range), TRUE);
    gtk_range_set_value (GTK_RANGE(Range), 100);
    gtk_widget_show (Range);
    
    // FIXME: for larger heights, the current value no
    // longer fits into the space provided by the range widget
    // better display it in status bar or some other place.
}

// dtor
GuiRenderHeight::~GuiRenderHeight()
{
    gtk_widget_destroy (Range);
    Range = NULL;
}

// update height of map
void GuiRenderHeight::setMapExtend (const s_int32 & min, const s_int32 & max)
{
    // any change at all?
    GtkAdjustment *bounds = gtk_range_get_adjustment (GTK_RANGE(Range));
    s_int32 cur_min = (s_int32) gtk_adjustment_get_lower (bounds);
    s_int32 cur_max = (s_int32) gtk_adjustment_get_upper (bounds);
    if (cur_min == min && cur_max == max) return;
    
    s_int32 cur_val = (s_int32) gtk_range_get_value (GTK_RANGE(Range));
    gtk_range_set_range (GTK_RANGE(Range), min, (min == max ? max + 100 : max));
    if (cur_val == cur_max)
    {
        // keep value at maximum if already there
        gtk_range_set_value (GTK_RANGE(Range), max);
    }
}
