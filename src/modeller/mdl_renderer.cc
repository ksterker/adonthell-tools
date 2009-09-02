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

#include "mdl_handle.h"
#include "mdl_renderer.h"

// ctor
ModelRenderer::ModelRenderer () : world::default_renderer ()
{
    ActiveShape = NULL;
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
        s_int16 x = (target->length() - sprt->length()) / 2;
        s_int16 y = (target->height() - sprt->height()) / 2;
        
        // draw to target
        draw (handles, x, y, ri, da, target);
        
        // render handles
        for (int i = 0; i < MAX_HANDLES; i++)
        {
            drawHandle (handles[i], false, da, target);
        }
    }
}

// draw cube outline(s)
void ModelRenderer::draw (GdkPoint *handles, const s_int16 & x, const s_int16 & y, const world::render_info & ri, const gfx::drawing_area & da, gfx::surface * target) const
{
    // render sprite
    renderer_base::draw (x, y, ri, da, target);

    // sprite screen coordinates
    s_int16 sx = x + ri.screen_x ();
    s_int16 sy = y + ri.screen_y ();
    
    // draw border around sprite
    u_int32 color = target->map_color (0xD8, 0x77, 0x2D);
    drawRect (sx, sy, ri.Sprite->length(), ri.Sprite->height(), color, da, target);
    
    // render shapes, if any, relative to sprite
    for (std::vector<world::cube3*>::const_iterator i = ri.Shape->begin(); i != ri.Shape->end(); i++)
    {
        s_int16 ox = sx - ri.Shape->ox();
        s_int16 oy = sy + (*i)->max_z() - (*i)->min_z() - ri.Shape->oy();

        (*i)->draw (ox, oy, &da, target);
        
        if (ActiveShape == *i)
        {
            ox += (*i)->min_x();
            oy += (*i)->min_y() - (*i)->min_z();
            
            updateHandles (handles, ox, oy);
        }
    }    
}

// update position of handles used for manipulating shapes
void ModelRenderer::updateHandles (GdkPoint *handles, const s_int16 & x, const s_int16 & y) const
{
    s_int16 length = ActiveShape->max_x() - ActiveShape->min_x();
    s_int16 width = ActiveShape->max_y() - ActiveShape->min_y();
    s_int16 height = ActiveShape->max_z() - ActiveShape->min_z();
    
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
