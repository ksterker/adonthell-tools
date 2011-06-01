/*
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file mapedit/gui_filter_dialog.h
 *
 * @author Kai Sterker
 * @brief Control filtering of entity list.
 */

#ifndef GUI_FILTER_DIALOG_H
#define GUI_FILTER_DIALOG_H

#include <gtk/gtk.h>

#include "gui_modal_dialog.h"
#include "gui_grid.h"

/**
 * Manipulate the grid to which objects will be aligned
 * when placing them onto the map.
 */
class GuiFilterDialog : public GuiModalDialog
{
public:

    /**
     * Create a singleton filter model used throughout the
     * lifetime of mapedit.
     */
    static GtkListStore *getFilterModel();

    /**
     * Create a dialog for the given grid.
     * @param filter_model model with the current filter settings.
     */
    GuiFilterDialog (GtkListStore *filter_model);
    
    /**
     * Hide the dialog and the grid.
     */
    virtual ~GuiFilterDialog ();
    
    /**
     * Called when the dialog is closed by pressing ESC.
     */
    void close ();
    
    static bool findTagInFilter (const gchar *tag, GtkTreeIter *result);

private:
    /// filter model instance used throughout mapedit
    static GtkListStore *FilterModel;

    /// button that toggles the grid dialog
    GtkWidget *GridControl;
    /// the user interface
    GtkBuilder *Ui;
};

#endif // GUI_FILTER_DIALOG_H
