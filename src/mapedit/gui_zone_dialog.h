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
 * @file mapedit/gui_zone_dialog.h
 *
 * @author Kai Sterker
 * @brief View and edit zone properties.
 */

#ifndef GUI_ZONE_DIALOG_H
#define GUI_ZONE_DIALOG_H

#include <world/zone.h>

#include "gui_modal_dialog.h"
#include "map_data.h"

/**
 * A dialog to display and edit map entity properties.
 */
class GuiZoneDialog : public GuiModalDialog
{
public:
    
    /**
     * Show dialog to view and edit the given zone.
     * @param z the zone to edit.
     * @param map area the zone belongs to.
     */
    GuiZoneDialog (world::zone *z, MapData *map);
    
    /**
     * Destructor.
     */
    virtual ~GuiZoneDialog();
    
    /**
     * Apply all changes made by the user to the underlying zone.
     * Called when the ok button has been pressed.
     */
    void applyChanges();
        
protected:
    void displayValues (const char & c);

    /// the zone being displayed or edited
    world::zone *Zone;
    /// the map the zone belongs to
    MapData *Map;
    /// the user interface
    GtkBuilder *Ui;
};

#endif
