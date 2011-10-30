/*
 $Id: map_renderer.h,v 1.2 2009/05/18 21:21:24 ksterker Exp $
 
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
 * @file mapedit/map_renderer.h
 *
 * @author Kai Sterker
 * @brief Special Renderer used by map editor.
 */

#ifndef MAP_RENDERER_H
#define MAP_RENDERER_H

#include <world/renderer.h>

/**
 * A renderer with additional functionalities used by the map editor.
 */
class MapRenderer : public world::default_renderer
{
public:
    /**
     * Create a new renderer used by map editor.
     */
    MapRenderer ();
    
    /**
     * Destructor.
     */
    virtual ~MapRenderer();

    /**
     * Get the object currently under the mouse pointer.
     * @param ox x-offset of map view 
     * @param oy y-offset of map view
     * @param mousePos the current mouse location.
     * @param objectlist the list of objects directly under the mouse.
     */
    world::chunk_info* findObjectBelowCursor (const s_int32 & ox, const s_int32 & oy, const GdkPoint *mousePos, const std::list <world::chunk_info*> & objectlist);
    
    /**
     * Remove selected object from the renderer.
     */
    void clearSelection ()
    {
        SelectedObject = NULL;
    }
    
protected:
    /**
     * Draw a single object to the screen.
     * @param x offset on the x-axis.
     * @param y offset on the y-axis.
     * @param obj the object to draw on screen.
     * @param da clipping rectangle.
     * @param target surface to draw on, NULL for screen surface.
     */
    virtual void draw (const s_int16 & x, const s_int16 & y, const world::render_info & obj, const gfx::drawing_area & da, gfx::surface * target) const;    

    /**
     * Check if the given render_info structure contains a part
     * that belongs to the given chunk_info.
     * @param ci an object (possibly composed of multiple parts)
     * @param ri an individual object part
     * @return true if part matches, false otherwise.
     */
    bool belongsToObject (const world::chunk_info *ci, const world::render_info *ri) const;

private:
    /// whether to gather selected object
    bool FindObjectBelowCursor;
    
    /// current location of the mouse pointer
    const GdkPoint *MousePos;
    /// the object currently pointed to
    world::chunk_info *SelectedObject;
};

#endif // MAP_RENDERER_H
