/*
   $Id: gui_modal_dialog.h,v 1.1 2009/03/29 12:27:25 ksterker Exp $

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
 * @file gui_modal_dialog.h
 *
 * @author Kai Sterker
 * @brief Base class for modal dialog windows
 */

#ifndef GUI_MODAL_DIALOG_H
#define GUI_MODAL_DIALOG_H

#include <gtk/gtk.h>

/**
 * Encapsulates the most basic functionality a modal/transient dialog window needs.
 */
class GuiModalDialog
{
public:
    /**
     * Initialize the dialog window.
     * @param p parent of the dialog.
     * @param m whether the dialog is model or just transient.
     */
    GuiModalDialog (GtkWindow *p, const bool & m = true);

    /*
     * Destroy the dialog window.
     */
    virtual ~GuiModalDialog ();

    /**
     * Displays the dialog window and sets it transient for the dlgedit main window.
     * @return the state of pressedOK. This should be set to <b>true</b> if the dialog
     * was closed via the OK button. By default it is <b>false</b>.
     */
    virtual bool run ();
    /**
     * Indicate whether the OK button of the dialog has been pressed.
     * @param button Set this to <b>true</b> if the OK button has been pressed,
     * otherwise to <b>false</b>
     */
    void okButtonPressed (bool button)  { pressedOK = button; }
    /**
     * Get the dialog window.
     * @return the dialog window.
     */
    GtkWidget *getWindow ()             { return window; }

protected:
    /// whether the Cancel or OK button has been pushed
    bool pressedOK;
    /// whether the dialog is truly modal or merely transient
    bool modal;
    /// the dialog window
    GtkWidget *window;              
    /// parent of the dialog.
    GtkWindow *parent;
};

#endif // GUI_MODAL_DIALOG_H
