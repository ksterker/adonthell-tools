/*
   $Id: main.cc,v 1.2 2006/03/19 20:27:19 ksterker Exp $
   
   Copyright (C) 1999/2000/2001/2002 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Dlgedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dlgedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file main.cc
 *
 * @author Kai Sterker
 * @brief The main function of dlgedit.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <iostream>
#include <locale.h>
#include "gettext.h"
#include "cfg_io.h"
#include "dlg_cmdline.h"
#include "dlg_compiler.h"
#include "gui_dlgedit.h"
#include "base/base.h"

#define main main

int main (int argc, char *argv[])
{
    // dlgedit configuration
    CfgIO config;
    
    // Init GTK+
    gtk_init (&argc, &argv);
    
    // parse command line
    if (!DlgCmdline::parse (argc, argv))
        return 1;

    // Init i18n
    setlocale (LC_ALL, "");
  
    // init game directory
    base::init (DlgCmdline::project, DlgCmdline::datadir);
    
    // start in gui mode
    if (!DlgCmdline::compile)
    {
        // Create the User Interface
        GuiDlgedit dlgedit;
        
        // are there any sources given?
        if (DlgCmdline::sources >= argc)
        {
            // Nope -> create a new dialogue
            dlgedit.newDialogue ();
        }
        else
        {
            // Yep -> load all of them
            while (DlgCmdline::sources < argc)
                dlgedit.loadDialogue (argv[DlgCmdline::sources++]);
        }
        
        // Start the main loop
        gtk_main ();
    }
    
    // just compile what we're given and exit
    else
    {
        DlgModule *module;
        std::string dialogue;
        
        while (DlgCmdline::sources < argc)
        {
            dialogue = argv[DlgCmdline::sources++];
            
            // check whether the file is a valid dialoge
            if (!GuiDlgedit::checkDialogue (dialogue))
            {
                std::cout << "Loading of '" << dialogue << "' failed\n";
                continue;
            }
            
            module = new DlgModule ("", dialogue, "-1", "");
            
            // try to load from file
            if (!module->load ())
            {
                std::cout << "Loading of '" << dialogue << "' failed\n";
            }
            else
            {            
                std::cout << "Compiling '" << dialogue << "' ...\n";
            
                // try to compile the dialogue
                DlgCompiler compiler (module);
    
                // compile
                compiler.run ();
            }
            
            delete module;
        }
    }
    
    // good bye
    return 0;
}
