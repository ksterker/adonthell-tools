/*
 $Id: screen_gtk.cc,v 1.1 2009/03/06 22:59:46 ksterker Exp $
 
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
#define gfx_screen_set_video_mode _gtk_LTX_gfx_screen_set_video_mode
#define gfx_screen_update _gtk_LTX_gfx_screen_update
#define gfx_screen_trans_color _gtk_LTX_gfx_screen_trans_color
#define gfx_screen_clear _gtk_LTX_gfx_screen_clear
#define gfx_screen_get_surface _gtk_LTX_gfx_screen_get_surface
#define gfx_screen_info _gtk_LTX_gfx_screen_info
#endif

#include "screen_gtk.h"

u_int32 trans_color = 0xFFFF00FF;

extern "C"
{
    bool gfx_screen_set_video_mode(u_int16 nl, u_int16 nh, u_int8 depth);
    void gfx_screen_update();
    u_int32 gfx_screen_trans_color();
    void gfx_screen_clear();
    gfx::surface *gfx_screen_get_surface();
    std::string gfx_screen_info();
}

bool gfx_screen_set_video_mode(u_int16 nl, u_int16 nh, u_int8 depth)
{
    // this is a noop
    return true;
}

void gfx_screen_update()
{
    // this is a noop
}

u_int32 gfx_screen_trans_color()
{
    return trans_color;
}

void gfx_screen_clear()
{
    // TODO
}

gfx::surface * gfx_screen_get_surface()
{
    return display;
}

std::string gfx_screen_info()
{
    return "GTK+ backend";
}
