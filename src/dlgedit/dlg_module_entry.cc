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
    
    itc = rpg::character::get_first ();
}

// reset everything to initial state
void DlgModuleEntry::clear ()
{
    init ();
}

// set the project
bool DlgModuleEntry::setProject (std::string p)
{
    bool retval = true;
    
    if (project_ != p)
    {
        project_ = p;
        
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
    bool result = rpg::character::load ();    
    itc = rpg::character::get_first();
    return result;
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

// check whether a certain character exists
bool DlgModuleEntry::isCharacter (const std::string &character)
{
    return rpg::character::get_character (character) != NULL;
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

    if (!rpg::character::is_last (itc))
    {
        // skip player character
        if (itc->second->type() == rpg::PLAYER)
        {
            itc++;
            return this->character();
        }

        character = itc->first;
        itc++;
    }
    else itc = rpg::character::get_first ();
    
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
