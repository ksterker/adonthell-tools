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
 * @file mapedit/gui_renderheight.h
 *
 * @author Kai Sterker
 * @brief Limit for rendering the map.
 */

#ifndef GUI_RENDERHEIGHT_H
#define GUI_RENDERHEIGHT_H

#include <gtk/gtk.h>
#include <adonthell/base/types.h>

/**
 * GUI element that allows the user to restrict rendering
 * to a maximum height. This causes all objects at greater
 * height to be hidden, allowing to edit what lies beneath.
 */
class GuiRenderHeight
{
public:
    /**
     * Create control.
     */
    GuiRenderHeight();
    
    /**
     * Clean up.
     */
    ~GuiRenderHeight();
    
    /**
     * Get widget displaying the render height.
     * @return the range widget.
     */
    GtkWidget *widget() const { return Range; }

    /**
     * Update the range as a new map gets assigned
     * or the height of the current map changes.
     * @param min the new lower bound of the map. 
     * @param max the new upper bound of the map. 
     */
    void setMapExtend (const s_int32 & min, const s_int32 & max);

    /**
     * Return the current value.
     * @return the current render limit.
     */
    s_int32 getLimit () const
    {
        return (s_int32) gtk_range_get_value (GTK_RANGE(Range));
    }
    
private:
    /// the range widget controlling the render height
    GtkWidget *Range;
};

#endif
