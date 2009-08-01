/*
 $Id: map_data.h,v 1.2 2009/04/03 22:00:48 ksterker Exp $
 
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
 * @file mapedit/map_data.h
 *
 * @author Kai Sterker
 * @brief Container for a map and associated data.
 */

#ifndef MAP_DATA_H
#define MAP_DATA_H

#include <world/area.h>

class MapData : public world::area 
{
public:
    /// iterator over entities on the map
    typedef std::vector<world::entity*>::iterator entity_iter;

    /**
     * Create a new map container.
     */
    MapData ();
    
    /**
     * Destructor.
     */
    virtual ~MapData();
    
    /**
     * @name Map Entities
     */
    //@{
    /**
     * Get the entity that has last been added to the map.
     * @return pointer to the newest entity present on the map. 
     */
    world::entity *getNewestEntity () const
    {
        return Entities.back();
    }
    
    /**
     * Get iterator to first entity on the map.
     * @return iterator starting at first entity.
     */
    entity_iter firstEntity() { return Entities.begin(); }
    
    /**
     * Get iterator pointing past last entity of the map.
     * @return iterator past last entity.
     */
    entity_iter lastEntity() { return Entities.end(); }
    
    /**
     * Try to rename the given entity. This will fail if an entity with the 
     * new name already exists on the map.
     * @param the entity to rename.
     * @param the new name for the entity.
     * @return NULL on failure, the new object otherwise.
     */
    world::placeable * renameEntity (world::named_entity *entity, const std::string & id);
    //@}    
    
    /**
     * @name Position Data
     */
    //@{
    /**
     * Set new X Position.
     * @param x the new X-Position.
     */
    void setX (const int & x) { PosX = x; }
    
    /**
     * Set new Y Position.
     * @param y the new Y-Position.
     */
    void setY (const int & y) { PosY = y; }

    /**
     * Set new Z Position.
     * @param z the new Z-Position.
     */
    void setZ (const int & z) { PosZ = z; }
    
    /**
     * Return current X-Position.
     * @return the X-Position.
     */
    int x () const { return PosX; }
    
    /**
     * Return current Y-Position.
     * @return the Y-Position.
     */
    int y () const { return PosY; }
    
    /**
     * Return current X-Position.
     * @return the X-Position.
     */
    int z () const { return PosZ; }    
    //@}
    
private:
    /// current x position of map in view
    int PosX;
    /// current x position of map in view
    int PosY;
    /// current x position of map in view
    int PosZ;    
};

#endif // MAP_DATA_H
