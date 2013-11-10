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

#include <adonthell/world/chunk_info.h>
#include <adonthell/world/coordinates.h>

#include "common/mdl_connector.h"

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
     * Cleanup.
     */
    ~MapEntity();

    /**
     * Load meta data for the contained object.
     */
    void loadMetaData ();

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
     * Check whether this entity at the given position intersects with any of the given objects.
     * @param objects list of objects to compare.
     * @param pos position of ourselves.
     * @return true if intersection is found, false otherwise.
     */
    bool intersects (const std::list<world::chunk_info*> & objects, const world::vector3<s_int32> & pos);

    /**
     * Check whether this entity contains the given tag.
     * @param tag the tag to search for.
     * @return true if the tag is found, false otherwise.
     */
    bool hasTag (const std::string & tag) const;

    /**
     * Check whether this entity shares a connector with the given entity.
     * @param entity the other entity
     * @param strict if true, names of connectors must match, otherwise,
     * only connector size is evaluated.
     * @return true if matching connectors exist, false otherwise.
     */
    bool canConnectWith (const MapEntity *entity, const bool & strict) const;

    /**
     * Calculate offset required to connect the two objects based on their
     * connectors.
     * @param entity the entity being connected
     * @param ox on completion contains the offset in x direction
     * @param oy on completion contains the offset in y direction
     */
    void connect (const MapEntity *entity, s_int32 & ox, s_int32 & oy) const;

    /**
     * @name Reference count
     *
     * Keeps track of how many instances of this object
     * exist on the map.
     */
    //@{
    /**
     * Call when object is added to the map.
     */
    void incRef ();
    /**
     * Call when object is removed from the map.
     */
    void decRef ();
    /**
     * Get number of times this object is present on the map.
     * @return the reference count.
     */
    u_int32 getRefCount () const                        { return RefCount; }
    //@}
        
    /**
     * @name Map related methods.
     */
    //@{
    /**
     * Add this entity to the given location.
     * @param pos the location to add object to.
     * @return true on success, false otherwise.
     */
    bool addToLocation(const world::coordinates & pos);

    /**
     * Remove this entity from its current location on the map.
     * @return true on success, false otherwise.
     */
    bool removeAtCurLocation ();
    
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
    bool isOnMap () const { return RefCount != 0; };    

    /**
     * Whether this entity can be placed onto the map.
     * @return true if placing is allowed, false otherwise.
     */
    bool canPlaceOnMap () const { return !Overlapping; }
    //@}
    
    /**
     * @name Name related methods.
     */
    //{
    /**
     * Create an unused id by adding a number to this entities' id.
     * @return an id that's not yet used on the entities map.
     */
    gchar* createNewId () const;
    
    /**
     * Change name of shared or unique entities. New name has to
     * be unique on the entities map.
     * @param id the new name.
     * @return true on success, false otherwise.
     */
    bool rename (const std::string & id);
    //@}
    
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

    /**
     * Get the comment associated with this object.
     * @return The comment associated with this object.
     */
    std::string get_comment () const { return Comment; }
    //@}

protected:
    /**
     * Remove this entities tags from filter.
     */
    void remove_tags ();

    /**
     * Calculate tags for this map entity and updates the filter model.
     */
    void update_tags ();

    /**
     * Add new tag to the entity
     * @param tag the tag to add.
     */
    void add_tag(const gchar *tag);

    /**
     * Check whether the given shape intersects with this map entity.
     * @param other_shape the shape to compare.
     * @param offset offset between the other entity and ourselves.
     * @return true if intersection is found, false otherwise.
     */
    bool intersects (const world::placeable_shape *other_shape, const world::vector3<s_int32> & offset) const;

private:
    /// number of times this object is present on the map
    u_int32 RefCount;
    /// whether the object is currently overlapping another object on the map
    bool Overlapping;

    /// the object this meta data is associated with 
    world::placeable *Object;
    /// the entity this meta data is associated with
    world::entity *Entity;
    /// position of entity on map (for named entities only)
    world::chunk_info *Location;

    /**
     * @name Meta-Data
     */
    //@{
    /// comment
    std::string Comment;
    /// list of tags associated with this entity
    std::vector<std::string> Tags;
    /// list of connectors for this entity
    std::vector<MdlConnector*> Connectors;
    //@}
};

#endif
