/*
   $Id: surface_gtk.h,v 1.2 2009/03/08 11:16:56 ksterker Exp $
 
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

#ifndef SURFACE_GTK_H
#define SURFACE_GTK_H

#include <cairo/cairo.h>
#include <adonthell/gfx/surface.h>

namespace gfx
{
    class surface_gtk : public surface
    {
    public:
        surface_gtk ();

        virtual ~surface_gtk (); 

        void resize (u_int16 l, u_int16 h);
        
        void clear ();

        void set_mask (bool m);

        void set_alpha (const u_int8 & surface_alpha, const bool & alpha_channel = false);

        void draw (s_int16 x, s_int16 y, s_int16 sx, s_int16 sy, u_int16 sl,
                           u_int16 sh, const drawing_area * da_opt = NULL,
                           surface * target = NULL) const;

        void fillrect (s_int16 x, s_int16 y, u_int16 l, u_int16 h,
                               u_int32 col, drawing_area * da_opt = NULL);

        void scale_up(surface *target, const u_int32 & factor) const;
        void scale_down(surface *target, const u_int32 & factor) const;

        void mirror(bool x, bool y);

        u_int32 map_color(const u_int8 & r, const u_int8 & g, const u_int8 & b, const u_int8 & a = 255) const;
        void unmap_color(u_int32 col, u_int8 & r, u_int8 & g, u_int8 & b, u_int8 & a) const;
        
        void lock () const;
        void unlock () const;
        
        void put_pix (u_int16 x, u_int16 y, u_int32 col);
        u_int32 get_pix (u_int16 x, u_int16 y) const;

        surface& operator = (const surface& src);

        /**
         * @name Loading / Saving to PNGs
         */
        //@{
        /** Loads an image from an opened file, in PNG format, without
         *  alpha and mask values.
         *  @param file the opened file from which to read.
         *  @return \e true in case of success, false otherwise.
         *  @sa load_png ()
         */
        bool get_png (std::ifstream & file);
        
        /** Saves an image into an opened file, in PNG format, without
         *  alpha and mask values.
         *
         *  @param file opened file where to save into.
         *  @return \e true in case of success, false otherwise.
         *  @sa save_png ()
         */
        bool put_png (std::ofstream & file) const;
        //@}
        
    protected:
        /// the actual image
        cairo_surface_t *vis;
        /// the mask image
        cairo_surface_t *mask;
        
        /**
         * Create the cairo context used to render onto this surface
         */
        cairo_t *create_drawing_context() const
        {
            return cairo_create (vis);
        }
        
    private: 
        /// clipping rectangles used in every blitting function.
        static gfx::drawing_area srcrect, dstrect; 

        /**
         *
         */
        void create_mask ();

        /** 
         * Used internally for blitting operations with drawing_areas.
         * @param x
         * @param y
         * @param sx
         * @param sy
         * @param sl
         * @param sh
         * @param draw_to
         */
        void setup_rects (s_int16 x, s_int16 y, s_int16 sx, s_int16 sy,
                          u_int16 sl, u_int16 sh, const drawing_area * draw_to) const; 
    };
}

#endif
