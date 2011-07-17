/*
   $Id: map_cmdline.h,v 1.1 2009/03/29 12:27:27 ksterker Exp $
   
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
 * @file mapedit/map_cmdline.h
 *
 * @author Kai Sterker
 * @brief Methods to parse the mapedit commandline.
 */

#ifndef MAP_CMDLINE_H
#define MAP_CMDLINE_H

#include <string>

/**
 * Apart from the above, MapCmdline stores the various options
 * that can be specified on the command line 
 */
class MapCmdline
{
public:
    /**
     * The method doing all the work. To be called right after
     * mapedit is launched.
     * @param argc argument count
     * @param argv argument vector
     * @return <b>false</b> indicates that the program shall quit
     */
    static bool parse (int argc, char* argv[]);

    /**
     * Detect the correct data and project directories from
     * path of a model being loaded.
     * @param file the model file name.
     */
    static bool setProjectFromPath (char *file);

    /**
     * The directory where mapedit searches for projects.
     */
    static std::string datadir;
    
    /**
     * The project. This is the directory that contains the model
     * directory.
     */
    static std::string project;
    
    /**
     * The directory where mapedit searches for models to
     * place on the map. The default is "models/"
     */
    static std::string modeldir;

    /**
     * The index in the argument vector pointing to the first non-option.
     * With a bit of luck, this is a map file to load on startup.
     */
    static int sources;
    
private:
    static void help (const std::string &program); 
};

#endif // MAP_CMDLINE_H
