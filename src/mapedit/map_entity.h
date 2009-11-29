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
#include <base/hash_map.h>
#include <world/chunk_info.h>

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
     * @param count refcount of the entity.
     */
    MapEntity (world::entity *obj, const u_int32 & count = 0);

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
     * Get the placeable wrapped by this object.
     * @return the placeable.
     */
    world::placeable *object () const { return Object; }
    
    /**
     * Create or update the wrapped entity. (For now, only the
     * Id can be updated.)
     * @param obj_type the placeable type.
     * @param entity_type type of the entity ('A', 'S' or 'U').
     * @param id unique identifier for the entity.
     * @return true on success, false otherwise.
     */
    bool update_entity (const world::placeable_type & obj_type, const char & entity_type, const std::string & id);
    
    /**
     * @name Map related attributes
     *
     */
    //@{
    /**
     * Call when object is added to the map.
     */
    void incRef ()                                      { RefCount++; }
    /**
     * Call when object is removed from the map.
     */
    void decRef ();
    /**
     * Get number of times this object is present on the map.
     * @return the reference count.
     */
    u_int32 getRefCount () const                        { return RefCount; }
    
    /**
     * Set location of this entity. For anonymous entities
     * this will be temporary.
     * @param location where this entity is placed on the map.
     */
    void setLocation ( world::chunk_info * location )   { Location = location; }
    
    /**
     * Get location of this entity. This might be NULL if the
     * entity is not part of the map or if the location is unknown.
     *
     * @return position of this entity on the map (or NULL).
     */
    world::chunk_info *getLocation() const              { return Location; }
    
    /**
     * Whether this entity is already present on the map.
     * Determines the background color of the row in the entity list.
     * @return true if object is part of the map, false otherwise.
     */
    bool is_on_map () const { return Entity != NULL; };    
    //@}
    
    /**
     * Create an unused id by adding a number to this entities' id.
     * @return an id that's not yet used on the entities map.
     */
    gchar* createNewId () const;
    
    /**
     * @name Meta-Data to display in GUI.
     */
    //@{
    /**
     * Return filename without path and file extension.
     * @return entity name.
     */
    gchar* get_name () const;

    /**
     * Return id of unique entities.
     * @return entity id or NULL.
     */
    gchar* get_id () const;

    /**
     * Get entity type.
     * @return one of A, S or U.
     */    
    gchar* get_entity_type () const;

    /**
     * Return the type of object encapsulated by the entity.
     * @return the map object type.
     */
    world::placeable_type get_object_type () const;

    /**
     * Return the states that are possible for this entity.
     * @return the object states.
     */
    std::hash_set<std::string> get_object_states () const;

    /**
     * Get thumbnail of the object.
     * @param size size of the rendered image.
     * @return rendering of the object at given size.
     */
    GdkPixbuf *get_icon (const u_int32 & size = 32) const;  
    //@}

protected:
    void update_tags ();
    
private:
    /// number of times this object is present on the map
    u_int32 RefCount;
        
    /// the object this meta data is associated with 
    world::placeable *Object;
    /// the entity this meta data is associated with
    world::entity *Entity;
    /// position of entity on map (for named entities only)
    world::chunk_info *Location;
    /// list of tags associated with this entity
    std::vector<std::string> Tags;
};

#endif
