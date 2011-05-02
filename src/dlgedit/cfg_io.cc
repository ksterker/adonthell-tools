/*
   $Id: cfg_io.cc,v 1.5 2008/09/19 18:09:39 ksterker Exp $

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
 * @file cfg_io.cc
 *
 * @author Kai Sterker
 * @brief I/O Routines for dlgedit configuration file
 */

#include <stdio.h>
#include <cstdlib>
#include <glib/gstdio.h>
#include "cfg_io.h"
#include "dlg_types.h"

// The config file opened by the lexical scanner
extern FILE* loadcfgin;

extern int parse_cfgfile (std::string&, int&);

// ctor; load config
CfgIO::CfgIO ()
{
#ifndef WIN32
    Dlgeditrc = std::string (getenv ("HOME")) + "/.adonthell/dlgeditrc";
#else
    Dlgeditrc = std::string ("./dlgeditrc");
#endif
    
    // loadcfgin is declared in lex.loadcfg.cc
    loadcfgin = g_fopen (Dlgeditrc.c_str (), "rb");

    // open succeeded -> read configuration
    if (loadcfgin)
    {
        load ();

        // cleanup
        fclose (loadcfgin);
    }
}

// dtor; save config
CfgIO::~CfgIO ()
{
    save ();    
}

// load config
void CfgIO::load ()
{
    int token = 1, n;
    std::string s;
    
    // as long as reading something from file ...
    while (token)
    {
        // get next token
        switch (token = parse_cfgfile (s, n))
        {
            case LOAD_PROJECT:
            {
                if (parse_cfgfile (s, n) == LOAD_STR) Data.addProject (s);
                break;
            }
            
            default: break;
        }
    }
    
    return;
}

// save config
void CfgIO::save ()
{
    // open file for writing
    std::ofstream out (Dlgeditrc.c_str ());
    
    // opening failed for some reasons    
    if (!out) return;
    
    // write header
    out << "# Adonthell Dialogue Editor configuration file\n" << std::endl;
            
    // save data
    Data.save (out);
} 
