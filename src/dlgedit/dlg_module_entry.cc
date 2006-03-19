/*
   $Id: dlg_module_entry.cc,v 1.2 2006/03/19 20:27:19 ksterker Exp $

   Copyright (C) 2002/2004 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file dlg_module_entry.cc
 *
 * @author Kai Sterker
 * @brief The textual contents of a DlgModule.
 */

#include <algorithm>
#include "dlg_cmdline.h"
#include "dlg_module_entry.h"
#include "base/base.h"
#include "rpg/quest.h"

// ctor
DlgModuleEntry::DlgModuleEntry ()
{
    init ();
}

// initialize the DlgModuleEntry to sane state
void DlgModuleEntry::init ()
{
    project_ = "none";
    imports_ = "";
    ctor_ = "";
    dtor_ = "";
    methods_ = "";
    
    itc = characters.end ();
}

// reset everything to initial state
void DlgModuleEntry::clear ()
{
    // empty character and quest array
    characters.clear ();
    
    init ();
}

// set the project
bool DlgModuleEntry::setProject (std::string p)
{
    bool retval = true;
    
    if (project_ != p)
    {
        project_ = p;
        
        // empty character and quest array
        characters.clear ();
        
        if (p != "none")
        {
            // set path
            base::Paths.init (p,  DlgCmdline::datadir);

            retval &= loadCharacters ();
            retval &= loadQuests ();
        }
    }
    
    return retval;
}

// load the character names
bool DlgModuleEntry::loadCharacters ()
{
    /*
    // look for a character file
    std::string file = game::find_file (project_ + "/character.data");
    if (file == "") return false;
    
    // open the file
    igzstream in;
    in.open (file);
    
    // version check
    if (!fileops::get_version (in, 3, 4, file))
        return false;
    
    // load characters
    character_base *mychar; 
    char ctemp;

    do
    {
        mychar = new character_base;
        mychar->get_state (in);
 
        // save the name of the NPC's
        if (mychar->get_id () != "Player") 
            addCharacter (mychar->get_name ());
        
        delete mychar;
    }
    while (ctemp << in);
    
    // close file
    in.close ();
    */
    itc = characters.begin ();
    
    return true; 
}

// load the quest names
bool DlgModuleEntry::loadQuests ()
{
    // clean previously loaded quest tree
    rpg::quest::cleanup ();
    
    // try loading quest tree
    if (!rpg::quest::get_state ())
    {
        fprintf (stderr, "*** failed loading 'quest.data' from '%s/%s'\n", 
                 DlgCmdline::datadir.c_str(), project_.c_str());
        return false;
    }

    return true; 
}

// add a character
void DlgModuleEntry::addCharacter (std::string character)
{
    std::vector<std::string>::iterator i;
    
    for (i = characters.begin (); i != characters.end (); i++)
        if (character < *i) break;
    
    characters.insert (i, character);
}

// check whether a certain character exists
bool DlgModuleEntry::isCharacter (const std::string &character)
{
    if (find (characters.begin (), characters.end (), character) == characters.end ())
        return false;
    
    return true;
}

// check whether a certain quest exists
bool DlgModuleEntry::isQuest (const std::string &quest)
{
    if (rpg::quest::get_part (quest) == NULL)
        return false;
    
    return true;
}

// iterate over the existing characters
std::string DlgModuleEntry::character ()
{
    std::string character = "";
    
    if (itc != characters.end ())
    {
        character = *itc;
        itc++;
    }
    else itc = characters.begin ();
    
    return character;
}

/*
// iterate over the existing quests
std::string DlgModuleEntry::quest ()
{
    std::string quest = "";
    
    if (itq != quests.end ())
    {
        quest = *itq;
        itq++;
    }
    else itq = quests.begin ();
    
    return quest;
}
*/
