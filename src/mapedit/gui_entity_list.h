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
 * @file mapedit/gui_entity_list.h
 *
 * @author Kai Sterker
 * @brief Display entities used on the map.
 */

#ifndef GUI_ENTITY_LIST_H
#define GUI_ENTITY_LIST_H

#include <gtk/gtktreeview.h>

#include "mapedit/map_data.h"

class GuiEntityList
{
public:
    /**
     * Create the user interface elements.
     */
    GuiEntityList ();
    
    /**
     * Set the map whose entities to display in the list.
     * @param map the map whose entities to display.
     */
    void setMap (MapData * map);
    
    /**
     * Get the tree view widget.
     * @return pointer to the tree view.
     */
    GtkWidget *getTreeWidget () const { return (GtkWidget*) TreeView; }
    
private:
    /// the map whose entities are displayed
    MapData* Map;
    /// the list view
    GtkTreeView *TreeView;
};

#endif
