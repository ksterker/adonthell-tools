/*
 $Id: main.cc,v 1.2 2009/05/18 21:21:24 ksterker Exp $
 
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
 * @file modeller/main.cc
 *
 * @author Kai Sterker
 * @brief The main function of modeller.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <base/base.h>
#include <gfx/gfx.h>

#include "mdl_cmdline.h"
#include "gui_modeller.h"

#define main main

int main (int argc, char *argv[])
{
    // Init GTK+
    gtk_init (&argc, &argv);
    
    // parse command line
    if (!MdlCmdline::parse (argc, argv))
        return 1;
    
    // init game directory
    base::init (MdlCmdline::project, MdlCmdline::datadir);
    
    // prepare a dummy configuration to avoid warnings
    base::configuration cfg;
    
    // Init GTK+ backend
    gfx::setup (cfg);
    if (!gfx::init ("gtk"))
        return 1;
    
    // Create the User Interface
    GuiModeller modeller;
    
    // are there any sources given?
    if (MdlCmdline::sources >= argc)
    {
        // Nope -> create a new model
        // modeller.newModel ();
    }
    else
    {
        // Yep -> load model
        // modeller.loadModel (argv[MapCmdline::sources]);
    }
    
    // Start the main loop
    gtk_main ();
    
    // good bye
    return 0;
}
