/*
   $Id: dlg_cmdline.cc,v 1.1 2004/07/25 15:52:22 ksterker Exp $
   
   Copyright (C) 2002 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file dlg_cmdline.cc
 *
 * @author Kai Sterker
 * @brief Methods to parse the dlgedit commandline.
 */
 
#include <iostream> 
#include <dirent.h>
#include <unistd.h>
#include "dlg_cmdline.h"

// flag indicating whether to compile the given scripts
bool DlgCmdline::compile = false;

// the directory to look for project files
std::string DlgCmdline::datadir = DATA_DIR"/games";

// the default project
std::string DlgCmdline::project = "none";

// index of the first dialgoue source in argv[]
int DlgCmdline::sources;

// examine the parameters passed to dlgedit
bool DlgCmdline::parse (int argc, char* argv[])
{
    int c;
    
    // Check for options
    while ((c = getopt (argc, argv, "cdhvp:j:")) != -1)
    {
        switch (c)
        {
            case 'd':
            {
                std::cout << datadir << std::endl;
                return false;
            }
                
            case 'v':
            {
                std::cout << _VERSION_ << std::endl;
                return false; 
            }
                
            case 'c':
            {
                compile = true;
                break;
            }
            
            case 'j':
            {
                project = optarg;
                break;
            }
            
            case 'p':
            {
                datadir = optarg;
                
                if (datadir[datadir.size () - 1] == '/')
                    datadir.erase (datadir.size () - 1);
                
                // Check whether the requested game directory exists
                DIR * mydir = opendir (datadir.c_str ()); 
                
                if (!mydir) 
                {
                    std::cerr << "No such directory " << datadir << "!" << std::endl;
                    return false;
                }
                closedir (mydir);
                
                break;
            }
            
            case '?':
            case 'h':
            {
                help (argv[0]);
                return false;
            }             
        }
    }
    
    sources = optind;
    return true;
}

// prints the help message
void DlgCmdline::help (const std::string &program)
{
    std::cout << "Usage: " << program << " [OPTIONS] [SOURCES]" << std::endl;
    std::cout << std::endl;
    std::cout << "Where [OPTIONS] can be:\n";
    std::cout << "-h         print this help message and exit" << std::endl; 
    std::cout << "-d         print the project directory and exit" << std::endl; 
    std::cout << "-v         print version and exit" << std::endl; 
    std::cout << "-c         compile all SOURCES and exit" << std::endl;
    std::cout << "-p dir     specify a custom project directory" << std::endl;
    std::cout << "-j project specify a default project" << std::endl;
}
