/*
   $Id: dlg_cmdline.h,v 1.1 2004/07/25 15:52:22 ksterker Exp $
   
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
 * @file dlg_cmdline.h
 *
 * @author Kai Sterker
 * @brief Methods to parse the dlgedit commandline.
 */

#ifndef DLG_CMDLINE_H
#define DLG_CMDLINE_H

#include <string>

/**
 * Apart from the above, DlgCmdline stores the various options
 * that can be specified on the command line 
 */
class DlgCmdline
{
public:
    /**
     * The method doing all the work. To be called right after
     * dlgedit is launched.
     * @param argc argument count
     * @param argv argument vector
     * @return <b>false</b> indicates that the program shall quit
     */
    static bool parse (int argc, char* argv[]);

    /**
     * The directory where dlgedit searches for projects.
     */
    static std::string datadir;
    
    /**
     * The project. This is the directory that contains the character-
     * and quest data dlgedit needs to (properly) compile dialogues
     * making use of advanced Python scripting features.
     */
    static std::string project;
    
    /**
     * This is set to <b>true</b> to indicate that only the dialogue
     * compiler should be run on the given sourcefiles, without launching
     * the GUI. After all files are compiled, dlgedit exits.
     */
    static bool compile;
    
    /**
     * The index in the argument vector pointing to the first non-option.
     * With a bit of luck, this is one or more dialogue sources.
     */
    static int sources;
    
private:
    static void help (const std::string &program); 
};

#endif // DLG_CMDLINE_H
