/*
   $Id: main.cc,v 1.2 2009/05/18 21:21:24 ksterker Exp $
   
   Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Mapedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Mapedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Mapedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file mapedit/main.cc
 *
 * @author Kai Sterker
 * @brief The main function of mapedit.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <base/base.h>
#include <gfx/gfx.h>
#include <python/python.h>
#include <rpg/character.h>

#include "map_cmdline.h"
#include "gui_mapedit.h"

#define main main

int main (int argc, char *argv[])
{
    // Init GTK+
    gtk_init (&argc, &argv);
    
    // parse command line
    if (!MapCmdline::parse (argc, argv))
        return 1;
  
    // init game directory
    base::init (MapCmdline::project, MapCmdline::datadir);

    // prepare a dummy configuration to avoid warnings
    base::configuration cfg;
    
    // Init GTK+ backend
    gfx::setup (cfg);
    if (!gfx::init ("gtk"))
        return 1;
    
    // Need python running for NPC schedule support
    python::init();

    // update the python search path
    python::add_search_path (base::Paths.game_data_dir());
    python::add_search_path (base::Paths.user_data_dir());
    
    // need to load world module
    python::import_module ("adonthell.world");
    
    // load character data
    rpg::character::load ();
    
    // Create the User Interface
    GuiMapedit mapedit;
        
    // are there any sources given?
    if (MapCmdline::sources >= argc)
    {
        // Nope -> create a new dialogue
        mapedit.newMap ();
    }
    else
    {
        // Yep -> load map
        mapedit.loadMap (argv[MapCmdline::sources]);
    }
    
    // Start the main loop
    gtk_main ();
    
    // good bye
    return 0;
}
