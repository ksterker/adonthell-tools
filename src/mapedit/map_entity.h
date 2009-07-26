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
 * Wrapper around an entity on the map, for storage in a
 * GtkListStore.
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
     * Create meta data container for object not yet 
     * placed on a map.
     * @param obj an object not present on the map.
     */
    MapEntity (world::placeable *obj);
  
    /**
     * Get the entity wrapped by this object.
     * @return the enitity or NULL if object not placed on a map yet.
     */
    world::entity *entity () const { return Entity; }
    
    /**
     * @name Meta-Data to display in GUI.
     */
    //@{
    /**
     * Return filename and (if applicable)
     * id of unique entities.
     * @return entity name and id.
     */
    gchar* get_name_and_id () const;

    /**
     * Get entity type.
     * @return one of A, S or U.
     */    
    gchar* get_type_name () const;
    
    /**
     * Get thumbnail of the object.
     * @return 32x32 rendering of the object.
     */
    GdkPixbuf *get_icon () const;
  
    /**
     * Whether this entity is already present on the map.
     * Determines the background color of the row in the entity list.
     * @return true if object is part of the map, false otherwise.
     */
    bool is_on_map () const { return Entity != NULL; };
    //@}

protected:
    void update_tags ();
    
private:
    /// the object this meta data is associated with 
    world::placeable *Object;
    /// the entity this meta data is associated with
    world::entity *Entity;
    /// list of tags associated with this entity
    std::vector<std::string> Tags;
};

#endif
