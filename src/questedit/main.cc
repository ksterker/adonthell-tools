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
 * @file questedit/main.cc
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

#include "qed_cmdline.h"
#include "gui_questedit.h"

#define main main

int main (int argc, char *argv[])
{
    // Init GTK+
    gtk_init (&argc, &argv);
    
    // parse command line
    if (!QedCmdline::parse (argc, argv))
        return 1;
    
    // init game directory
    base::init (QedCmdline::project, QedCmdline::datadir);
    
    // create the User Interface
    GuiQuestedit questedit;
    
    // are there any sources given?
    if (QedCmdline::sources >= argc)
    {
        // Nope -> a new quest tree is created
    }
    else
    {
        // Yep -> load quest data
        // questedit.loadData (argv[QedCmdline::sources]);
    }
    
    // start the main loop
    gtk_main ();
    
    // good bye
    return 0;
}
