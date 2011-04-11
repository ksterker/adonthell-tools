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
 * @file modeller/mdl_renderer.cc
 *
 * @author Kai Sterker
 * @brief A renderer for single shapes
 */

#include <gfx/gfx.h>

#include "mdl_handle.h"
#include "mdl_renderer.h"

// ctor
ModelRenderer::ModelRenderer () : world::default_renderer ()
{
    ActiveShape = NULL;
    ActiveModel = NULL;
    Overlay = gfx::create_surface();
    Overlay->set_alpha(96, true);
}

void ModelRenderer::render (std::list <world::render_info> & objectlist, const gfx::drawing_area & da, gfx::surface * target) const
{
    // reset overlay
    Overlay->resize (target->length(), target->height());
    Overlay->fillrect (0, 0, target->length(), target->height(), 0);

    // center on screen
    s_int16 x = X_AXIS_POS;
    s_int16 y = target->height() / 2;
    
    // render
    default_renderer::render (x, y, objectlist, da, target);
    
    // finish rendering
    Overlay->draw (0, 0, &da, target);
}

// render model and handles
void ModelRenderer::render (world::placeable_model *model, GdkPoint *handles, const gfx::drawing_area & da, gfx::surface *target)
{
    gfx::sprite *sprt = model->get_sprite();
    if (sprt != NULL)
    {
        // information required for rendering
        std::vector<world::shadow_info> shadow;
        world::render_info ri (model->current_shape(), sprt, world::vector3<s_int32>(), &shadow);
        
        // center on screen
        s_int16 x = X_AXIS_POS;
        s_int16 y = target->height() / 2;
        
        // draw to target
        draw (handles, x, y, ri, da, target);
        
        // render handles
        for (int i = 0; i < MAX_HANDLES; i++)
        {
            drawHandle (handles[i], false, da, target);
        }
    }
}

// draw with translucency
void ModelRenderer::draw (const s_int16 & x, const s_int16 & y, const world::render_info & obj, const gfx::drawing_area & da, gfx::surface * target) const
{
    if (ActiveModel != NULL && obj.Shape == ActiveModel->current_shape())
    {
        // draw everything behind the currently selected sprite
        Overlay->draw (0, 0, &da, target);
        // clear overlay
        Overlay->fillrect (0, 0, Overlay->length(), Overlay->height(), 0);
        // draw currently selected sprite
        renderer_base::draw (x, y, obj, da, target);        
    }
    else
    {
        // render unselected sprites onto translucent overlay
        renderer_base::draw (x, y, obj, da, Overlay);        
    }
}

const world::cube3* ModelRenderer::scaleCube (const world::cube3 *src) const
{
    world::cube3 *result = new world::cube3 (0, 0, 0);

    for (int i = 0; i < world::cube3::NUM_CORNERS; i++)
    {
        world::vector3<s_int16> p = src->get_point(i);

        // we need the explicit cast, because vector3::operator*
        // returns a vector3 of the operand type, which in case
        // of base::Scale is u_int8. Way too small for us!
        result->set_point(i, p * ((s_int16) base::Scale));
    }

    result->create_bounding_box();
    return result;
}

// draw cube outline(s)
void ModelRenderer::draw (GdkPoint *handles, const s_int16 & x, const s_int16 & y, const world::render_info & ri, const gfx::drawing_area & da, gfx::surface * target) const
{
    // sprite screen coordinates
    s_int16 sx = x + ri.screen_x () * base::Scale;
    s_int16 sy = y + ri.screen_y () * base::Scale;
    
    // draw border around sprite
    u_int32 color = target->map_color (0xD8, 0x77, 0x2D);
    drawRect (sx, sy, ri.Sprite->length() * base::Scale, ri.Sprite->height() * base::Scale, color, da, target);
    
    // render shapes, if any, relative to sprite
    for (std::vector<world::cube3*>::const_iterator i = ri.Shape->begin(); i != ri.Shape->end(); i++)
    {
        const world::cube3 *c = scaleCube (*i);
        c->draw (x, y, &da, target);
        
        if (ActiveShape == *i)
        {
            s_int16 ox = x + c->min_x();
            s_int16 oy = y + c->min_y() - c->min_z();
            
            updateHandles (handles, ox, oy);
        }

        delete c;
    }    
}

// update position of handles used for manipulating shapes
void ModelRenderer::updateHandles (GdkPoint *handles, const s_int16 & x, const s_int16 & y) const
{
    s_int16 length = (ActiveShape->max_x() - ActiveShape->min_x()) * base::Scale;
    s_int16 width = (ActiveShape->max_y() - ActiveShape->min_y()) * base::Scale;
    s_int16 height = (ActiveShape->max_z() - ActiveShape->min_z()) * base::Scale;
    
    // move position (x/y/z) handle in the top north-west corner of the shape
    handles[POSITION].x = x - HANDLE_OFFSET;
    handles[POSITION].y = y - HANDLE_OFFSET - height;

    // change length handle in the top east vertice of the shape
    handles[LENGTH].x = x - HANDLE_OFFSET + length;
    handles[LENGTH].y = y - HANDLE_OFFSET - height + width / 2;

    // change width handle in the top south vertice of the shape
    handles[WIDTH].x = x - HANDLE_OFFSET + length / 2;
    handles[WIDTH].y = y - HANDLE_OFFSET - height + width;

    // change height handle in the top north vertice of the shape
    handles[HEIGHT].x = x - HANDLE_OFFSET + length / 2;
    handles[HEIGHT].y = y - HANDLE_OFFSET - height;
}

// render a shape handle
void ModelRenderer::drawHandle (const GdkPoint & handle, const bool & highlight, const gfx::drawing_area & da, gfx::surface * target) const
{
    if (ActiveShape != NULL)
    {
        s_int16 sx = handle.x;
        s_int16 sy = handle.y;
        
        u_int32 color = highlight ? target->map_color (0x98, 0xC7, 0x0D) : target->map_color (0xC8, 0xC7, 0x2D);
        u_int32 frame = highlight ? target->map_color (0xFF, 0xFF, 0xFF) : target->map_color (0xDD, 0xDD, 0xDD);

        // draw handle with border
        target->fillrect (sx, sy, HANDLE_SIZE, HANDLE_SIZE, color, NULL);
        drawRect (sx, sy, HANDLE_SIZE, HANDLE_SIZE, frame, da, target);
    }
}

// draw a rectangle
void ModelRenderer::drawRect (const s_int16 & x, const s_int16 & y, const u_int16 & l, const u_int16 & h, const u_int32 & color, const gfx::drawing_area & da, gfx::surface * target) const
{
    target->draw_line (x, y, x + l, y, color, &da);
    target->draw_line (x, y, x, y + h, color, &da);
    target->draw_line (x + l, y, x + l, y + h, color, &da);
    target->draw_line (x, y + h, x + l, y + h, color, &da);    
}
