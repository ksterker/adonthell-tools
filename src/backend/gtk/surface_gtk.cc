/*
    $Id: surface_gtk.cc,v 1.5 2009/05/21 14:28:18 ksterker Exp $

    Copyright (C) 2009 Kai Sterker <kai.sterker@gmail.com>
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


#include <iostream>

#include "surface_gtk.h"
#include "screen_gtk.h"

using gfx::surface_gtk;

gfx::drawing_area surface_gtk::srcrect;
gfx::drawing_area surface_gtk::dstrect; 

// ctor
surface_gtk::surface_gtk() : surface () 
{ 
    vis = NULL;
    mask = NULL;
}

// dtor
surface_gtk::~surface_gtk() 
{
    clear ();
}

// cleanup
void surface_gtk::clear () 
{
    if (vis) cairo_surface_destroy (vis);
    if (mask) cairo_surface_destroy (mask);
    
    vis = NULL;
    mask = NULL;
    
    set_length (0);
    set_height (0); 
    set_alpha (255);
    set_mask (false);
}

// set mask
void surface_gtk::set_mask (bool m)
{
    if (m != is_masked ())
    {
        is_masked_ = m;
        
        // prepare mask
        if (m)
        {
            create_mask ();
        }
        else
        {
            cairo_surface_destroy (mask);
            mask = NULL;
        }
    }
}

// set per surface or per pixel alpha
void surface_gtk::set_alpha (const u_int8 & t, const bool & alpha_channel)
{
    // add alpha channel if surface has none, yet
    if (vis && alpha_channel && cairo_surface_get_content (vis) != CAIRO_CONTENT_COLOR_ALPHA)
    {
        cairo_surface_t* tmp = cairo_surface_create_similar (vis, CAIRO_CONTENT_COLOR_ALPHA, length(), height());
        cairo_t* cr = cairo_create (tmp);
        cairo_set_source_surface (cr, vis, 0, 0);
        cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        cairo_fill (cr);
        cairo_destroy (cr);
        cairo_surface_destroy (vis);
        vis = tmp;
    }
    
    alpha_ = t;
    alpha_channel_ = alpha_channel;
}

// render (part of) surface
void surface_gtk::draw (s_int16 x, s_int16 y, s_int16 sx, s_int16 sy, u_int16 sl,
                        u_int16 sh, const drawing_area * da_opt,
                        surface * target) const
{ 
    // create target drawing context
    const surface_gtk * target_gtk;
    if (target == NULL) target_gtk = (const surface_gtk *) display;
    else target_gtk = (const surface_gtk *) target;
    cairo_t *cr = target_gtk->create_drawing_context();

    // prepare clipping rectangles
    setup_rects (x, y, sx, sy, sl, sh, da_opt); 
    if (!cr || !dstrect.length() || !dstrect.height())
        return;

    // check if clipping will occur
    bool clippingRequired = false;
    if (srcrect.x() > x + sx || srcrect.length() < sl ||
        srcrect.y() > y + sy || srcrect.height() < sh)
    {
        cairo_push_group (cr);
        clippingRequired = true;
    }
        
    // we'll have two distinct drawing operations
    if (is_masked () && alpha() != 255)
    {
        cairo_push_group (cr);
    }
    
    // set source surface
    cairo_set_source_surface (cr, vis, dstrect.x(), dstrect.y());

    if (is_masked () && alpha() != 255)
    {
        // draw masked image with per-surface alpha
        cairo_mask_surface (cr, mask, dstrect.x(), dstrect.y());
        cairo_pop_group_to_source (cr);
        cairo_paint_with_alpha (cr, alpha()/255.0);
    }
    else if (is_masked ())
    {
        // draw masked image
        cairo_mask_surface (cr, mask, dstrect.x(), dstrect.y());
    }
    else if (alpha () != 255)
    {
        // draw per-surface alpha
        cairo_paint_with_alpha (cr, alpha()/255.0);
    }
    else /* (!is_masked() && alpha() == 255) */
    {
        // draw per-pixel alpha
        cairo_paint (cr);        
    }
    
    if (clippingRequired)
    {       
        // draw clipped source to target
        cairo_pop_group_to_source (cr);
        cairo_rectangle (cr, srcrect.x(), srcrect.y(), srcrect.length(), srcrect.height());
        cairo_fill (cr);
    }
        
    // cleanup
    cairo_destroy (cr);
}

// fill a rectangle with given color
void surface_gtk::fillrect (s_int16 x, s_int16 y, u_int16 l, u_int16 h, u_int32 col, drawing_area * da_opt)
{
    u_int8 r, g, b, a;
    unmap_color (col, r, g, b, a);

    cairo_t* cr = create_drawing_context ();
    cairo_set_source_rgba (cr, r/255.0, g/255.0, b/255.0, a/255.0);
    cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
    
    if (da_opt) 
    {
        drawing_area da = da_opt->setup_rects (); 
        cairo_rectangle (cr, da.x(), da.y(), da.length(), da.height());
    }
    else
    { 
        cairo_rectangle (cr, x, y, l, h);
    }

    cairo_fill (cr);    
    cairo_destroy (cr);
}

// convert RGBA color to surface format
u_int32 surface_gtk::map_color (const u_int8 & r, const u_int8 & g, const u_int8 & b, const u_int8 & a) const
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

// convert surface color format into RGBA
void surface_gtk::unmap_color(u_int32 col, u_int8 & r, u_int8 & g, u_int8 & b, u_int8 & a) const
{
    a = (col & 0xFF000000) >> 24;
    r = (col & 0x00FF0000) >> 16;
    g = (col & 0x0000FF00) >> 8;
    b = (col & 0x000000FF);
}

// lock surface
void surface_gtk::lock () const
{
    if (!length () || !height ()) return;
    cairo_surface_flush (vis);
}

// unlock surface
void surface_gtk::unlock () const
{
    if (!length () || !height ()) return;
    cairo_surface_mark_dirty (vis);
}

// set pixel to given color
void surface_gtk::put_pix (u_int16 x, u_int16 y, u_int32 col) 
{
    u_int32 stride = cairo_image_surface_get_stride (vis);
    u_int32 offset = y * stride + x * 4;
    u_int8 *pixels = cairo_image_surface_get_data (vis);
    
    switch (cairo_image_surface_get_format (vis))
    {
        case CAIRO_FORMAT_ARGB32:
        {
            *((u_int32 *) (pixels + offset)) = col;
            break;
        }
        case CAIRO_FORMAT_RGB24:
        {
            *((u_int32 *) (pixels + offset)) = col | 0xFF000000;
            break;
        }
        default:
        {
            break;
        }
    }    
}

// get color of given pixel
u_int32 surface_gtk::get_pix (u_int16 x, u_int16 y) const
{
    u_int32 stride = cairo_image_surface_get_stride (vis);
    u_int32 offset = y * stride + x * 4;
    u_int8 *pixels = cairo_image_surface_get_data (vis);
    
    u_int32 color;
    switch (cairo_image_surface_get_format (vis))
    {
        case CAIRO_FORMAT_ARGB32:
        {
            color = *((u_int32 *) (pixels + offset));
            break;
        }
        case CAIRO_FORMAT_RGB24:
        {
            color = 0xff000000 | *((u_int32 *) (pixels + offset));
            break;
        }
        default:
        {
            color = 0;
            break;
        }
    }
    
    return color;
}

// copy surface
gfx::surface & surface_gtk::operator = (const surface& src)
{
    const surface_gtk & src_gtk = (const surface_gtk &) src;

    // copy size and attributes
    (drawable&) (*this) = (drawable&) src;
    alpha_channel_ = src.has_alpha_channel();
    is_masked_ = src.is_masked();
    alpha_ = src.alpha();
    
    // free current surface and mask
    if (vis) cairo_surface_destroy (vis);
    if (mask) cairo_surface_destroy (mask);
    
    // copy image
    if (!src_gtk.vis) vis = NULL;
    else 
    {
        cairo_content_t type = alpha_channel_ ? CAIRO_CONTENT_COLOR_ALPHA : CAIRO_CONTENT_COLOR;
        vis = cairo_surface_create_similar (src_gtk.vis, type, src_gtk.length(), src_gtk.height());
        cairo_t* cr = cairo_create (vis);
        cairo_set_source_surface (cr, src_gtk.vis, 0, 0);
        cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        cairo_fill (cr);
        cairo_destroy (cr);
    }
    
    // copy mask
    if (!is_masked_) mask = NULL;
    else
    {
        mask = cairo_surface_create_similar (src_gtk.mask, CAIRO_CONTENT_ALPHA, src_gtk.length(), src_gtk.height());
        cairo_t* cr = cairo_create (mask);
        cairo_set_source_surface (cr, src_gtk.mask, 0, 0);
        cairo_set_operator (cr, CAIRO_OPERATOR_SOURCE);
        cairo_fill (cr);
        cairo_destroy (cr);
    }
        
    return *this; 
}

// set or update surface size
void surface_gtk::resize (u_int16 l, u_int16 h)
{
    if (l == length () && h == height ()) return;

    if (vis) cairo_surface_destroy (vis);

    set_length (l);
    set_height (h); 

    // create surface with per-pixel alpha?
    cairo_format_t format = alpha_channel_ ? CAIRO_FORMAT_ARGB32 : CAIRO_FORMAT_RGB24;

    vis = cairo_image_surface_create (format, l, h);        
    if (is_masked()) create_mask ();
}

// mirror this surface
void surface_gtk::mirror (bool x, bool y)
{
    int mx = 1;
    int my = 1;
    
    if (x)
    {
        mx = -1;
        is_mirrored_x_ = !is_mirrored_x_;
    }
    
    if (y)
    {
        my = -1;
        is_mirrored_y_ = !is_mirrored_y_;
    }
    
    if (x || y)
    {
        cairo_t* cr = cairo_create (vis);
        cairo_pattern_t* pattern = cairo_pattern_create_for_surface (vis);
        cairo_matrix_t matrix;
        cairo_matrix_init_scale (&matrix, mx, my);
        cairo_pattern_set_matrix (pattern, &matrix);
        cairo_set_source (cr, pattern);
        cairo_paint (cr);    
        cairo_destroy (cr);
        
        // update mask
        if (mask)
        {
            create_mask ();
        }
    }
}

// read png data from stream
static cairo_status_t read_png (void *closure, unsigned char *data, unsigned int length)
{
    std::ifstream &file = *(std::ifstream *) closure;
    file.read ((char*) data, length);
    return file.fail() ? CAIRO_STATUS_READ_ERROR : CAIRO_STATUS_SUCCESS;
}

// write png data to stream
static cairo_status_t write_png (void *closure, unsigned char *data, unsigned int length)
{
    std::ofstream &file = *(std::ofstream *) closure;
    file.write ((char*) data, length);
    return file.fail() ? CAIRO_STATUS_WRITE_ERROR : CAIRO_STATUS_SUCCESS;
}

// load surface from png image
bool surface_gtk::get_png (std::ifstream & file)
{
    clear ();
    
    vis = cairo_image_surface_create_from_png_stream ((cairo_read_func_t) read_png, &file);
    if (vis)
    {
        set_length (cairo_image_surface_get_width (vis));
        set_height (cairo_image_surface_get_height (vis)); 
    }
    
    return vis != NULL;
}


// save surface as png image
bool surface_gtk::put_png (std::ofstream & file) const
{
    cairo_status_t result = cairo_surface_write_to_png_stream (vis, (cairo_write_func_t) write_png, &file);
    return result == CAIRO_STATUS_SUCCESS;
}

// create a mask for the surface
void surface_gtk::create_mask ()
{
    if (mask) cairo_surface_destroy (mask);
    
    mask = cairo_image_surface_create (CAIRO_FORMAT_A8, length(), height());
    u_int8 *mask_data = cairo_image_surface_get_data (mask);
    
    u_int32 mask_color = gfx::screen::trans_color ();

    cairo_surface_flush (mask);
    cairo_surface_flush (vis);
    
    for (int l = 0; l < length(); l++)
    {
        for (int h = 0; h < height(); h++)
        {
            u_int32 color = get_pix (l, h);
            if (color != mask_color) mask_data[h*length()+l] = 255;
        }
    }
    
    cairo_surface_mark_dirty (mask);
}

// setup clipping area
void surface_gtk::setup_rects (s_int16 x, s_int16 y, s_int16 sx, s_int16 sy,
                               u_int16 sl, u_int16 sh, const drawing_area * draw_to) const
{
    if (draw_to)
    { 
        drawing_area im_zone (x + sx, y + sy, sl, sh);        
        im_zone.assign_drawing_area (draw_to);
        srcrect = im_zone.setup_rects ();
    }
    else
    {
        srcrect = drawing_area (x + sx, y + sy, sl, sh);
    }
    
    dstrect = srcrect;
    dstrect.move (x, y);
}

