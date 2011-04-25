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
 * @file mapedit/gui_grid_dialog.h
 *
 * @author Kai Sterker
 * @brief Control grid for aligning object placement.
 */

#ifndef GUI_GRID_DIALOG_H
#define GUI_GRID_DIALOG_H

#include <gtk/gtk.h>

#include "gui_modal_dialog.h"
#include "gui_grid.h"

/**
 * Manipulate the grid to which objects will be aligned
 * when placing them onto the map.
 */
class GuiGridDialog : public GuiModalDialog, public GridMonitor
{
public:
    /**
     * Create a dialog for the given grid.
     * @param grid the grid to manipulate.
     * @param ctrl button that toggles the dialog.
     */
    GuiGridDialog (GuiGrid *grid, GtkWidget *ctrl);
    
    /**
     * Hide the dialog and the grid.
     */
    virtual ~GuiGridDialog ();
    
    /**
     * Callback to get notified of changes
     * to the grid.
     */
    virtual void gridChanged();
    
    /**
     * Callback to get notified of changes
     * to the auto-adjust object.
     */
    virtual void objChanged ();

    /**
     * Update grid with user supplied values.
     */
    void updateGrid ();
    
    /**
     * Update object alignment.
     * @param activated whether button was toggled on or off.
     * @param counterpart opposite of the button just toggled.
     */
    void updateAlignment (const bool & activated, const gchar *counterpart);

    /**
     * Toggle whether the grid will automatically
     * adjust to the selected object.
     * @param auto_adjust true to adjust grid to object.
     */
    void setAutoAdjust (const bool & auto_adjust);

    /**
     * Toggle whether objects placed on the map will
     * be aligned to the grid.
     * @param snap_to_grid true to align objects with grid.
     */
    void setSnapToGrid (const bool & snap_to_grid);

    /**
     * Called when the dialog is closed by pressing ESC.
     */
    void close ();
    
private:
    /// the grid manipulated by the grid dialog
    GuiGrid *Grid;
    /// button that toggles the grid dialog
    GtkWidget *GridControl;
    /// whether the grid is currently changing
    bool GridChanging;
    /// the user interface
    GtkBuilder *Ui;
};

#endif // GUI_GRID_DIALOG_H
