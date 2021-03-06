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
 * @file questedit/qed_cmdline.h
 *
 * @author Kai Sterker
 * @brief Methods to parse the questedit commandline.
 */

#ifndef QED_CMDLINE_H
#define QED_CMDLINE_H

#include <string>

/**
 * Apart from the above, QedCmdline stores the various options
 * that can be specified on the command line 
 */
class QedCmdline
{
public:
    /**
     * The method doing all the work. To be called right after
     * questedit is launched.
     * @param argc argument count
     * @param argv argument vector
     * @return <b>false</b> indicates that the program shall quit
     */
    static bool parse (int argc, char* argv[]);

    /**
     * The directory where questedit searches for projects.
     */
    static std::string datadir;
    
    /**
     * The project. This is the directory that contains the mapobjects
     * that can be placed on the map.
     */
    static std::string project;
    
    /**
     * The index in the argument vector pointing to the first non-option.
     * With a bit of luck, this is a map file to load on startup.
     */
    static int sources;
    
private:
    /**
     * Print help message.
     * @param program the program name.
     */
    static void help (const std::string &program); 
};

#endif // QED_CMDLINE_H
