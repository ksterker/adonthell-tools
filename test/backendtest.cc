 /*
   $Id: backendtest.cc,v 1.3 2009/04/04 19:08:29 ksterker Exp $

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

#include <unistd.h>
#include <gtk/gtk.h>

#include <adonthell/base/base.h>
#include <adonthell/gfx/gfx.h>
#include <adonthell/world/area_manager.h>
#include <adonthell/world/character.h>
#include <adonthell/world/object.h>
#include <adonthell/world/mapview.h>

#include "backend/gtk/screen_gtk.h"

class game_client
{
public:
    world::area world;
    world::character *mchar;
	
    game_client()
    {
        mchar = NULL;
    }

    void create_map ()
    {
        // Adding map objects
        world::object * mobj;

		// short grass, 1x1 at index 0
        mobj = new world::object (world, "0");
        world.add_entity(new world::entity(mobj));
        mobj->load_model("models/map/ground/outside/short-grass-tile.xml");
        
        // long grass, 1.5x1.5 at index 1
        mobj = new world::object (world, "1");
        mobj->load_model("models/map/ground/outside/long-grass-tile.xml");
        world.add_entity(new world::entity(mobj));

        // wooden planks, 1x1 at index 2
        mobj = new world::object (world, "2");
        mobj->load_model("models/map/ground/outside/wood-1.xml");
        world.add_entity(new world::entity(mobj));

        // wooden pole, left, height 40 at index 3
        mobj = new world::object (world, "3");
        mobj->load_model("models/map/ground/outside/wood-pole-l.xml");
        world.add_entity(new world::entity(mobj));

        // wooden pole, right, height 40 at index 4 
        mobj = new world::object (world, "4");
        mobj->load_model("models/map/ground/outside/wood-pole-r.xml");
        world.add_entity(new world::entity(mobj));

        // diagonal cliff, 40x45x75 at index 5
        mobj = new world::object (world, "5");
        mobj->load_model("models/map/wall/outside/cliff-se.xml");
        world.add_entity(new world::entity(mobj));

        // diagonal cliff top, 40x45x5 at index 6
        mobj = new world::object (world, "6");
        mobj->load_model("models/map/wall/outside/cliff-se-top.xml");
        world.add_entity(new world::entity(mobj));
        
        // cliff facing south, 80x5x80 at index 7
        mobj = new world::object (world, "7");
        mobj->load_model("models/map/wall/outside/cliff-s.xml");
        world.add_entity(new world::entity(mobj));

        // Adding the map character at index 8
        mchar = new world::character (world, "8");
        mchar->world::placeable::load_model("models/char/npc/ng.xml");
        world.add_entity(new world::named_entity(mobj, "Player"));

        // set position and speed
        mchar->set_position (398, 322);
        mchar->set_z (0);
        
        // put character on map
        world.place_entity(8, *mchar);

        world::coordinates mc;
        
        // create ground (grass tiles are 40x40)
        for (u_int16 i = 0; i < 16; i++)
            for (u_int16 j = 0; j < 3; j++)
            {
                world::coordinates mc (i*40, j*40, 80);
                world.place_entity (0, mc);
            }
                
        for (u_int16 j = 0; j < 3; j++)
            for (u_int16 i = 0; i < 3-j; i++)
            {
                world::coordinates mc (i*40, (j+3)*40, 80);
                world.place_entity (0, mc);
            }
                        
        
		// 4 poles (left side)
        mc.set_x(400);mc.set_y(170);
        world.place_entity(3, mc);  // that one is actually invisible
        mc.set_x(400);mc.set_y(240);
        world.place_entity(3, mc);
        // (right side)
        mc.set_x(470);mc.set_y(170);  // that one is actually invisible
        world.place_entity(4, mc);
        mc.set_x(470);mc.set_y(240);
        world.place_entity(4, mc);

		// wooden platform
        for (int i = 10; i < 12; i++)
        {
            for (int j = 4; j < 6; j++)
            {
                world::coordinates mc (i*40, j*40, 40);
                world.place_entity (2, mc);
            }
        }

		// 4 wooden poles
        mc.set_x(280);mc.set_y(280);
        world.place_entity(3, mc);
        mc.set_z(40);
        world.place_entity(3, mc);
        mc.set_x(280);mc.set_y(170);
        mc.set_z(0);
        world.place_entity(3, mc);
        mc.set_z(40);
        world.place_entity(3, mc);
        
        mc.set_z(0);
        mc.set_x(350);mc.set_y(280);
        world.place_entity(4, mc);
        mc.set_z(40);
        world.place_entity(4, mc);
        mc.set_x(350);mc.set_y(170);
        mc.set_z(0);
        world.place_entity(4, mc);
        mc.set_z(40);
        world.place_entity(4, mc);

		// wooden platform
        for (int i = 7; i < 9; i++)
        {
            for (int j = 4; j < 7; j++)
            {
                world::coordinates mc (i*40, j*40, 80);
                world.place_entity (2, mc);
            }
        } 

		// "stair"
        for (int i = 4; i < 17; i++)
        {
            world::coordinates mc ((i/2) * 40 + (i%2) * 20, 360, 5 * (i-4));
            world.place_entity (2, mc);
        }

        world::coordinates mc2 (120, 200, 0);
        world.place_entity (2, mc2);
        
        // create ground (grass tiles are 60x60, but grid is 40x40)
        for (float i = 0; i < 16; i += 1.5)
            for (float j = 2; j < 12; j += 1.5)
            {
                u_int16 x = (u_int16) (40 * i);
                u_int16 y = (u_int16) (40 * j);
                
                world::coordinates mc (x, y, 0);
                world.place_entity (1, mc);
            }
        
        
        // create diagonal wall
        for (int i = 0; i < 4; i++)
        {
            world::coordinates mc (i*40, (6-i)*40, 0);
            world.place_entity (5, mc);
            mc.set_z (80);
            world.place_entity (6, mc);
        }
        
        // create straight wall
        for (int i = 4; i < 16; i+=2)
        {
            world::coordinates mc (i*40, 120, 0);
            world.place_entity (7, mc);
        }

        world.save("/tmp/backendtest.amap");
        world::area_manager::set_active_map("/tmp/backendtest.amap");
    }
};

// Redraw the screen from the backing pixmap
gint expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    // the renderer ...
    world::default_renderer rndr;
    world::mapview mv (800, 600, &rndr);
    mv.center_on (320, 240);
    
    // this is a GTK+ backed "screen" surface
    gfx::screen_surface_gtk *target = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    target->set_drawable (gtk_widget_get_window(widget));
    target->fillrect (0, 0, 800, 600, 0xFF000000);
    
    // test clipping
    gfx::drawing_area da (200, 150, 400, 300);
    
    // render mapview to screen
    mv.draw (25, 25, &da, target);
    
    // rectangle that should be filled with the mapview
    target->fillrect (200, 150, 400, 2, 0xFFFF8888);
    target->fillrect (200, 300+150, 400, 2, 0xFFFF8888);
    target->fillrect (200, 150, 2, 300, 0xFFFF8888);
    target->fillrect (400+200, 150, 2, 300, 0xFFFF8888);

    return FALSE;
}    

int main (int argc, char *argv[])
{
    int c;
    std::string userdatadir = "";
    
    // Check for options
    while ((c = getopt (argc, argv, "g:")) != -1)
    {
        switch (c)
        {
            // user supplied data directory:
            case 'g':
                userdatadir = optarg;
                break;
            default:
                break;
        }
    }
    
    // Init GTK+
    gtk_init (&argc, &argv);

    // Init base module
    base::init ("", userdatadir);
    base::configuration cfg;

    // Init event module
    events::init(cfg);

    // Init GTK+ backend
    gfx::setup (cfg);
    gfx::init ("gtk");
    
    // Contains map
    game_client gc;
    
    // Create game world
    gc.create_map();    
    
    // Main Window
    GtkWidget *wnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (wnd), 800, 600);
    gtk_widget_realize (wnd);
    
    // Drawing area
    GtkWidget *graph = gtk_drawing_area_new ();
    gtk_widget_set_size_request (graph, 800, 600);
    g_signal_connect (G_OBJECT (graph), "expose_event", G_CALLBACK (expose_event), &gc);
    gtk_widget_set_events (graph, GDK_EXPOSURE_MASK /*| GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK*/);
    
    gtk_container_add (GTK_CONTAINER (wnd), graph);
    
    // Make visible
    gtk_widget_show_all (wnd);
    
    // Start the main loop
    gtk_main ();
    
    return 0;
}
