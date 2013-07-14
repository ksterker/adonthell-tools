/*
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


#ifndef GFX_GTK_SCREEN_H
#define GFX_GTK_SCREEN_H

#include <gdk/gdk.h>

#include <adonthell/gfx/screen.h>
#include "surface_gtk.h"

/// the color that appears transparent when the surface is masked
extern u_int32 trans_color;

namespace gfx
{
    
/** 
 * The glue between backend and gtk
 */
class screen_surface_gtk : public gfx::surface_gtk
{
public:
    screen_surface_gtk () : surface_gtk () 
    { 
        Drawable = NULL; 
    }
    
    virtual ~screen_surface_gtk() 
    { 
        clear(); 
    }
    
    void resize (u_int16 l, u_int16 h) 
    { 
        set_length (l);
        set_height (h);
    }

    void clear () 
    { 
        if (Drawable)
        {
            g_object_unref (Drawable);
            Drawable = NULL;
        }
    }
    
    void set_drawable (GdkDrawable *drawable) 
    { 
        clear ();

        gint l, h;
        if (GDK_IS_WINDOW (drawable))
        {
            l = gdk_window_get_width (GDK_WINDOW (drawable));
            h = gdk_window_get_height (GDK_WINDOW (drawable));
        }
        else if (GDK_IS_PIXMAP (drawable))
        {
            gdk_pixmap_get_size (GDK_PIXMAP (drawable), &l, &h);
        }
        else
        {
            fprintf (stderr, "*** screen::set_drawable: Invalid drawable: %s!\n",
                    drawable ? G_OBJECT_TYPE_NAME(drawable) : "<null>");
            return;
        }

        set_length(l);
        set_height(h);

        g_object_ref (drawable);
        Drawable = drawable;
    }
    
protected:
    cairo_t *create_drawing_context() const
    {
        if (Drawable) return gdk_cairo_create (Drawable);
        fprintf (stderr, "*** screen::create_drawing_context: No drawable to draw on!\n");
        return NULL;
    }

private:
    /// the gtk window to draw on.
    GdkDrawable *Drawable;
};


}

/// the "screen" instance
extern gfx::screen_surface_gtk *display;

#endif // GFX_GTK_SCREEN_H
