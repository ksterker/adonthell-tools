/*
   $Id: gfx_gtk.cc,v 1.1 2009/03/06 22:59:46 ksterker Exp $

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

#ifdef USE_LIBTOOL
/* exported names for libltdl */
#define gfx_init _gtk_LTX_gfx_init
#define gfx_cleanup _gtk_LTX_gfx_cleanup
#define gfx_create_surface _gtk_LTX_gfx_create_surface
#endif

#include "surface_gtk.h"
#include "screen_gtk.h"

extern "C"
{
    bool gfx_init();
    void gfx_cleanup();

    gfx::surface * gfx_create_surface();
}

gfx::screen_surface_gtk *display = NULL;

bool gfx_init()
{
    display = new gfx::screen_surface_gtk();
    return true;
}

void gfx_cleanup()
{
    delete display;
}

gfx::surface * gfx_create_surface()
{
    return new gfx::surface_gtk();
}
