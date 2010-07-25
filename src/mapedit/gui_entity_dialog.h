/* 
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file mapedit/gui_entity_dialog.h
 *
 * @author Kai Sterker
 * @brief View and edit entity properties.
 */

#ifndef GUI_ENTITY_DIALOG_H
#define GUI_ENTITY_DIALOG_H

#include "gui_modal_dialog.h"
#include "map_entity.h"

namespace world
{
    class character;
}

class GuiScriptSelector;

/**
 * A dialog to display and edit map entity properties.
 */
class GuiEntityDialog : public GuiModalDialog
{
public:
    typedef enum
    {
        UPDATE_PROPERTIES,
        ADD_ENTITY_TO_MAP,
        DUPLICATE_NAMED_ENTITY
    } Mode;
    
    /**
     * Show dialog to view and edit the given entity.
     * @param entity a map object.
     */
    GuiEntityDialog (MapEntity *entity, const GuiEntityDialog::Mode & mode);
    
    /**
     * Destructor.
     */
    virtual ~GuiEntityDialog();
    
    /**
     * Apply all changes made by the user to the underlying entity.
     * Called when the ok button has been pressed.
     */
    void applyChanges();
    
    /**
     * @name Entity attribute update.
     */
    //@{
    /**
     * Set the object type. Required for adding 
     * a new entity to the map.
     * @param type the placeable type.
     */
    void set_object_type (const world::placeable_type & type);

    /**
     * Set the entity type. Required for adding 
     * a new entity to the map.
     * @param type 'A', 'U' or 'S'.
     */
    void set_entity_type (const char & type);
    
    /**
     * Set the entity state, i.e. the animation (but possibly also
     * the internal state).
     * @param state name of the state to set/display.
     */
    void set_entity_state (const std::string & state);
    
    void setLocation (world::chunk_info *location);
    //@}
    
protected:
    /**
     * Enable or disable a page of the dialog.
     * @param page the page number
     * @param active true to enable page, false to disable
     */
    void set_page_active (const int & page, const bool & active);
    
    /**
     * Populate the list of locations at which the entity exists.
     */
    void setLocations ();

    /**
     * Initialize scenery-specific page(s).
     * @param location the object's location.
     */
    void init_from_scenery (world::chunk_info *location);

    /**
     * Initialize character-specific page(s).
     * @param chr the character instance to pick values from. 
     */
    void init_from_character (world::character *chr);

    /**
     * Store data from scenery-specific page(s).
     * @param location the scenery instance to update.
     */
    void set_scenery_data (world::chunk_info *location);

    /**
     * Store data from character-specific page(s).
     * @param chr the character instance to update.
     */
    void set_character_data (world::character *chr);
    
    /// mode in which the dialog has been opened
    Mode DlgMode;
    /// the object being displayed or edited
    MapEntity *Entity;
    /// the entity state
    std::string EntityState;
    /// the object type
    world::placeable_type ObjType;
    /// the entities type
    char EntityType;
    /// the user interface
    GtkBuilder *Ui;
    /// Python script helper
    GuiScriptSelector *Selector;
};

#endif
