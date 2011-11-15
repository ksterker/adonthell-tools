/*
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
 
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream> 
#include <dirent.h>
#include <unistd.h>

#include "base/logging.h"

#include "common/util.h"
#include "map_cmdline.h"

// the directory to look for project files
std::string MapCmdline::datadir = DATA_DIR"/games";

// the default project
std::string MapCmdline::project = "";

// the default project
std::string MapCmdline::modeldir = "models";

// index of the first dialgoue source in argv[]
int MapCmdline::sources;

// examine the parameters passed to dlgedit
bool MapCmdline::parse (int argc, char* argv[])
{
    int c;
    
    // Check for options
    while ((c = getopt (argc, argv, "dhvg:p:m:")) != -1)
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
            
            case 'm':
            {
                modeldir = optarg;

                if (modeldir[modeldir.size () - 1] == '/')
                    modeldir.erase (modeldir.size () - 1);

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

// determine project and data directories from file name
bool MapCmdline::setProjectFromPath (char *file)
{
    // try to get an absolute path first
    std::string strpath = util::get_absolute_path(file);

    // now find the project directory in the path
    size_t idx;
    struct stat statbuf;
    while (stat ((strpath + "/maps").c_str (), &statbuf))
    {
        idx = strpath.rfind ("/");
        if (idx == std::string::npos)
        {
            LOG(ERROR) << "Failed detecting data directory.";
            return false;
        }

        strpath = strpath.substr(0, idx);
    }

    // the directory before that is our working directory
    idx = strpath.rfind('/');
    if (idx == std::string::npos)
    {
        LOG(ERROR) << "Failed detecting project.";
        return false;
    }

    project = strpath.substr(idx + 1);
    datadir = strpath.substr(0, idx);

    LOG(WARNING) << "Detected datadir " << datadir;
    LOG(WARNING) << "Detected project " << project;
    return true;
}

// prints the help message
void MapCmdline::help (const std::string &program)
{
    std::cout << "Usage: " << program << " [OPTIONS] [MAPFILE]" << std::endl;
    std::cout << std::endl;
    std::cout << "Where [OPTIONS] can be:\n";
    std::cout << "-h         print this help message and exit" << std::endl; 
    std::cout << "-d         print the default projects directory and exit" << std::endl;
    std::cout << "-v         print version and exit" << std::endl; 
    std::cout << "-g path    specify path to custom projects directory (default is builtin)" << std::endl;
    std::cout << "-p project specify project inside projects directory" << std::endl;
    std::cout << "-m dir     specify directory to load models from (default is models)" << std::endl;
}
