/*
   $Id: map_cmdline.cc,v 1.1 2009/03/29 12:27:27 ksterker Exp $
   
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
 * @file mapedit/map_cmdline.cc
 *
 * @author Kai Sterker
 * @brief Methods to parse the mapedit command line.
 */
 
#include <iostream> 
#include <dirent.h>
#include <unistd.h>
#include "map_cmdline.h"

// the directory to look for project files
std::string MapCmdline::datadir = DATA_DIR"/games";

// the default project
std::string MapCmdline::project = "";

// index of the first dialgoue source in argv[]
int MapCmdline::sources;

// examine the parameters passed to dlgedit
bool MapCmdline::parse (int argc, char* argv[])
{
    int c;
    
    // Check for options
    while ((c = getopt (argc, argv, "dhvg:p:")) != -1)
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
                
            case 'p':
            {
                project = optarg;
                break;
            }
            
            case 'g':
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
void MapCmdline::help (const std::string &program)
{
    std::cout << "Usage: " << program << " [OPTIONS] [SOURCES]" << std::endl;
    std::cout << std::endl;
    std::cout << "Where [OPTIONS] can be:\n";
    std::cout << "-h         print this help message and exit" << std::endl; 
    std::cout << "-d         print the project directory and exit" << std::endl; 
    std::cout << "-v         print version and exit" << std::endl; 
    std::cout << "-g dir     specify a custom project directory" << std::endl;
    std::cout << "-p project specify a default project" << std::endl;
}
