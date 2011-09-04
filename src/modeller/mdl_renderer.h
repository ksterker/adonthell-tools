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
 * @file modeller/mdl_renderer.h
 *
 * @author Kai Sterker
 * @brief A renderer for single shapes
 */


#ifndef MDL_RENDERER_H
#define MDL_RENDERER_H

#include <gdk/gdk.h>
#include <world/renderer.h>

#include "common/mdl_connector.h"
#include "mdl_handle.h"

#define X_AXIS_POS 80

/**
 * Renders a single shape using the logic of the in-game renderer.
 */
class ModelRenderer : public world::default_renderer
{
public:
    /**
     * Create a new instance of the renderer.
     */
    ModelRenderer ();
    
    /**
     * Destructor.
     */
    virtual ~ModelRenderer () {}
    
    void render (std::list <world::render_info> & objectlist, const gfx::drawing_area & da, gfx::surface * target) const;

    void draw (const s_int16 & x, const s_int16 & y, const world::render_info & obj, const gfx::drawing_area & da, gfx::surface * target) const;

    /**
     * Render the given shape.
     * @param offset the scroll offset.
     * @param model the model being currently edited.
     * @param handles the handles for the currently edited shape.
     * @param da clipping rectangle.
     * @param target surface to draw on.
     */
    void render (const GdkPoint & offset, world::placeable_model *model, ModelHandles *handles, const gfx::drawing_area & da, gfx::surface *target);

    void render (const GdkPoint & offset, std::list <world::render_info> & objectlist, MdlConnector *connector, const gfx::drawing_area & da, gfx::surface *target);

    /**
     * Draw the handle at given index.
     * @param handle the handle to draw.
     * @param highlight whether to highlight the handle.
     * @param da clipping rectangle.
     * @param target canvas to draw on.
     */
    void drawHandle (const GdkPoint & handle, const bool & highlight, const gfx::drawing_area & da, gfx::surface * target) const;
    
    /**
     * Set the model that is being edited, so we can draw it specially.
     * @param model the model being edited or NULL to reset.
     */
    void setActiveModel (world::placeable_model *model)
    {
        ActiveModel = model;
    }
    
    /**
     * Set the shape that is being edited, so we can draw the handles.
     * @param cube the cube being edited or NULL to reset.
     */
    void setActiveShape (world::cube3 *cube)
    {
        ActiveShape = cube;
    }
    
protected:
    /**
     * Draw model and update handle positions.
     * @param handles handle positions.
     * @param x x-offset
     * @param y y-offset
     * @param ri the model information
     * @param da clipping rectangle
     * @param target render target
     */
    void draw (ModelHandles *handles, const s_int16 & x, const s_int16 & y, const world::render_info & ri, const gfx::drawing_area & da, gfx::surface * target) const;

    /**
     * Draw a rectangular outline.
     * @param x x-offset
     * @param y y-offset
     * @param l length of rectangle
     * @param h height of rectangle
     * @param da clipping rectangle
     * @param target render target
     */
    void drawRect (const s_int16 & x, const s_int16 & y, const u_int16 & l, const u_int16 & h, const u_int32 & color, const gfx::drawing_area & da, gfx::surface * target) const;

    /**
     * Return a scaled copy of the given cube, using
     * the scale factor of base::Scale.
     * @param the cube to copy and scale.
     * @return a new cube object. Delete when no longer required.
     */
    const world::cube3* scaleCube (const world::cube3 *src) const;

private:
    /// this is the model currently being edited
    world::placeable_model *ActiveModel;
    /// this is the shape currently being edited
    world::cube3 *ActiveShape;
    /// additional surface for rendering with translucency
    gfx::surface *Overlay;
};

#endif
