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
 * @file mapedit/map_entity.h
 *
 * @author Kai Sterker
 * @brief Meta data for entities used on the map.
 */

#ifndef MAP_ENTITY_H
#define MAP_ENTITY_H

#include <gdk-pixbuf/gdk-pixbuf.h>
#include <world/entity.h>

/**
 *
 */
class MapEntity
{
public:
    /**
     * Create meta data container for map entity.
     * @param obj an entity on the map.
     */
    MapEntity (world::entity *obj);
    
    /**
     * @name Meta-Data to display in GUI.
     */
    //@{
    gchar* get_name_and_id () const;
    
    gchar* get_type_name () const;
    
    GdkPixbuf *get_icon () const;
    //@}

protected:
    void update_tags ();
    
private:
    /// the entity this meta data is associated with
    world::entity *Entity;
    /// list of tags associated with this entity
    std::vector<std::string> Tags;
};

#endif
