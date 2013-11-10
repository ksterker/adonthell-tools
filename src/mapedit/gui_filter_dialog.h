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

#include <adonthell/base/callback.h>

#include "gui_modal_dialog.h"
#include "gui_grid.h"

class MapEntity;

/**
 * Manipulate the grid to which objects will be aligned
 * when placing them onto the map.
 */
class GuiFilterDialog : public GuiModalDialog
{
public:
    typedef enum
    {
        NONE,
        BY_TAG,
        BY_CONNECTOR
    } filter_type;

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

    /**
     * Update the filter currently used.
     * @param name name of the radio button representing the filter.
     */
    void setFilter (const std::string & name);

    /**
     * Turn filtering off, temporarily.
     */
    static void pauseFiltering ();

    /**
     * Turn filtering back on.
     */
    static void resumeFiltering ();

    /**
     * Return the filter that is currently active.
     * @return the filter that is currently active.
     */
    static filter_type getActiveFilter () { return ActiveFilter; }

    /**
     * Create a singleton filter model used throughout the
     * lifetime of mapedit.
     */
    static GtkListStore *getFilterModel();

    /**
     * Check whether the given entity should be filtered from the entity list
     * or not.
     * @param entity the entity to check.
     * @return true to filter the entity, false otherwise.
     */
    static bool isEntityFiltered (const MapEntity *entity);
    
    /**
     * Check if the given tag is already contained in the list of all tags.
     * If it is found, out parameter result is set to an iterator pointing
     * at the row that contains the tag.
     * @param tag the tag to find
     * @param result iterator pointing at tag, if found
     * @return true on success, false otherwise.
     */
    static bool findTagInFilter (const gchar *tag, GtkTreeIter *result);

protected:
    /**
     * Show an entity if it contains at least one of the enabled tags.
     * @param entity the entity to filter
     * @return true to hide entity from list, false otherwise.
     */
    static bool filterByTag (const MapEntity *entity);

    /**
     * Show an entity if it shares at least one connector with the
     * entity currently selected for placing onto the map.
     * @param entity the entity to filter
     * @return true to hide entity from list, false otherwise.
     */
    static bool filterByConnector (const MapEntity *entity);

private:
    /// filter model instance used throughout mapedit
    static GtkListStore *FilterModel;
    /// the function used to filter the entity list
    static base::functor_1ret<const MapEntity*, bool> *FilterFunc;
    /// the currently active filter
    static filter_type ActiveFilter;
    /// whether filtering is suspended
    static bool Paused;

    /// the user interface
    GtkBuilder *Ui;
};

#endif // GUI_FILTER_DIALOG_H
