/*
 $Id: screen_gtk.h,v 1.3 2009/04/03 22:00:19 ksterker Exp $
 
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

#include "gfx/screen.h"
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
        Window = NULL; 
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
        if (Window)
        {
            g_object_unref (Window);
            Window = NULL;
        }
    }
    
    void set_window (GdkWindow *wnd) 
    { 
        clear ();
        
        int l, h;
        gdk_drawable_get_size (wnd, &l, &h);
        set_length (l);
        set_height (h);
        
        g_object_ref (wnd);
        Window = wnd;
    }
    
protected:
    virtual cairo_t *create_drawing_context() const
    {
        if (Window) return gdk_cairo_create (GDK_DRAWABLE(Window));
        fprintf (stderr, "*** screen::create_drawing_context: No window to draw on!\n");
        return NULL;
    }

private:
    /// the gtk window to draw on.
    GdkWindow *Window;
};


}

/// the "screen" instance
extern gfx::screen_surface_gtk *display;

#endif // GFX_GTK_SCREEN_H
