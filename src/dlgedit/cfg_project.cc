/*
   $Id: cfg_project.cc,v 1.1 2004/07/25 15:52:22 ksterker Exp $
   
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
 * @file cfg_project.cc
 *
 * @author Kai Sterker
 * @brief Project specific configuration data
 */

#include "cfg_project.h"
#include "dlg_types.h"

extern int parse_cfgfile (std::string&, int&);

// ctor
CfgProject::CfgProject (std::string name)
{
    Name = name;
    BaseDir = "";
}

// load project data
bool CfgProject::load ()
{
    int token = 1, n;
    std::string s;
    
    // as long as reading something from file ...
    while (token)
    {
        // get next token
        switch (token = parse_cfgfile (s, n))
        {
            case LOAD_END: return true;
            
            case LOAD_BASE_DIR:
            {
                if (parse_cfgfile (s, n) == LOAD_STR) BaseDir = s;
                break;
            }
            
            default: break;
        }
    }
    
    return false;
}

// save project data
void CfgProject::save (std::ofstream &out)
{
    out << "\nProject [" << Name << "]\n"
        << "  BaseDir [" << BaseDir << "]\n"
        << "End" << std::endl;
}
