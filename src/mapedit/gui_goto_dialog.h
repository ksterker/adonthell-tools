/* 
 Copyright (C) 2010 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file mapedit/gui_goto_dialog.h
 *
 * @author Kai Sterker
 * @brief View and edit goto properties.
 */

#ifndef GUI_GOTO_DIALOG_H
#define GUI_GOTO_DIALOG_H

#include "gui_modal_dialog.h"

/**
 * A dialog to go to a specified location on the map.
 */
class GuiGotoDialog : public GuiModalDialog
{
public:
    /**
     * Show dialog to go to the specified location.
     */
    GuiGotoDialog ();
    
    /**
     * Destructor.
     */
    virtual ~GuiGotoDialog();

    /**
     * Go to selected position
     */
    void gotoPosition ();

protected:
    /// the user interface
    GtkBuilder *Ui;
};

#endif
