/*
   $Id: dlg_circle_entry.cc,v 1.1 2004/07/25 15:52:22 ksterker Exp $

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
 * @file dlg_circle_entry.cc
 *
 * @author Kai Sterker
 * @brief The contents of a DlgCircle.
 */
 
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <algorithm>
#include "gettext.h"
#include "gui_dlgedit.h"
#include "dlg_circle_entry.h"

// Default Constructor
DlgCircleEntry::DlgCircleEntry ()
{
    loop_ = false;
    text_ = "";
    annotation_ = "";
    npc_ = "Default";
    code_ = "";
    condition_ = "";
}

// get the dialogue text
std::string DlgCircleEntry::text ()
{
    // in case we are in preview mode, we need to translate the text first
    if (GuiDlgedit::window && GuiDlgedit::window->mode () == L10N_PREVIEW) 
        return gettext (text_.c_str ());

    // usually, we just need to return the text
    return text_;    
}

// set the dialogue text
void DlgCircleEntry::setText (std::string t)
{
    unsigned int pos = 0;
    text_ = t;

    // Make text safe:
    // replace linebreaks with space
    replace (text_.begin (), text_.end (), '\n', ' ');

    // escape quotes
    while ((pos = text_.find ("\"", pos)) != text_.npos)
    {
        if (pos > 0) {
            if (text_[pos-1] != '\\') text_.insert (pos, "\\");
        }
        else text_.insert (pos, "\\");

        pos++;
    }
}

// set the node's condition
void DlgCircleEntry::setCondition (std::string c)
{
    if (c == "")
    {
        condition_ = c;
        return;    
    }
    
    // remove all whitespace from the edges of the condition
    condition_ = c.substr (c.find_first_not_of (" \n\t"), c.find_last_not_of (" \n\t") + 1);

    // replace 'else if' by the shorter 'elif'
    if (strncmp ("else if", condition_.c_str (), 7) == 0)
        condition_.replace (0, 7, "elif");
}
