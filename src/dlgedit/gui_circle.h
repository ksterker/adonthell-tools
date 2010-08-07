/*
   $Id: gui_circle.h,v 1.2 2009/03/29 12:27:25 ksterker Exp $ 

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
 * @file gui_circle.h
 *
 * @author Kai Sterker
 * @brief The Edit Circle dialog
 */

#ifndef GUI_CIRCLE_H
#define GUI_CIRCLE_H

#include "dlg_circle_entry.h"
#include "dlg_module_entry.h"
#include "dlg_types.h"
#include "gui_edit.h"
#include "gui_modal_dialog.h"

/**
 * This is the dialog used to input text, conditions and arbitrary code of
 * a DlgCircle. It further is used to specify whether the node contains speech
 * of the Player, the Narrator or of an NPC. In latter case, the text may
 * be attached to a special NPC, or to the default one. This default one is
 * always the NPC to whom the dialogue belongs in the game.
 */
class GuiCircle : public GuiModalDialog
{
public:
    /**
     * Open the "Edit Dialogue Node" dialog. 
     * @param parent The parent of the dialog
     * @param type The type of the circle to display (NPC, PLAYER or NARRATOR)
     * @param entry The contents of the circle
     * @param dme The contents of the dialogue
     */
    GuiCircle (GtkWindow *parent, node_type *type, DlgCircleEntry *entry, DlgModuleEntry *dme);

    /**
     * Apply the values entered in the dialogue to the edited node. This
     * method should be called when the user hits the OK button.
     */
    void applyChanges ();

private:
    gchar *getOption (GtkComboBox * cbox);
    void setOption (GtkOptionMenu *o, const gchar *label);

    GuiEdit *cond_edit;             // Text entry for conditions
    GuiEdit *code_edit;             // Text entry for code
    GtkWidget *loop;                // Checkbox for looping text
    GtkTextBuffer *text_entry;      // Entry for dialogue text
    GtkTextBuffer *annotation_entry;// Entry for annotations
    GtkComboBox *npc_selection;     // The dropdown list with the various NPC's
    DlgCircleEntry *entry;          // The data to display
    node_type *type;                // The type of the node
};

#endif // GUI_CIRCLE_H
