/*
 $Id: map_data.cc,v 1.2 2009/04/03 22:00:48 ksterker Exp $
 
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
 * @file mapedit/map_data.cc
 *
 * @author Kai Sterker
 * @brief Container for a map and associated data.
 */

#include "map_data.h"
#include "map_entity.h"

// ctor
MapData::MapData() : world::area ()
{
    PosX = 0;
    PosY = 0;
    PosZ = 0;
}

// dtor
MapData::~MapData()
{
}

// count how often the given object is present on the map
u_int32 MapData::getEntityCount (world::entity *ety) const
{
    u_int32 count = 0;
    std::list<world::chunk_info*> result;
    std::list<world::chunk_info*>::const_iterator i;
    
    objects_in_bbox (min(), max(), result, ety->get_object()->type());
    for (i = result.begin(); i != result.end(); i++)
    {
        if ((*i)->get_entity() == ety) count++;
    }
    
    return count;
}

// check for named entity presence
bool MapData::exists (const std::string & entity_name)
{
    std::hash_map<std::string, world::named_entity*>::const_iterator result = NamedEntities.find (entity_name);
    return (result != NamedEntities.end());
}

// rename map entity
world::entity* MapData::renameEntity (MapEntity *entity, const std::string & id)
{
    // first, check if the new name is valid
    if (exists (id))
    {
        // TODO: display message in status bar
        return NULL;
    }
    
    // remove entity under its current name
    world::named_entity *ety = (world::named_entity *) entity->entity();
    std::string *name = (std::string*) ety->id();
    NamedEntities.erase (*name);
    
    // set renamed entity
    name->replace(name->begin(), name->end(), id);
    NamedEntities[id] = ety;
    
    return ety;
}

// delete (unused) entity from map
void MapData::remove_entity (MapEntity *entity)
{
    if (entity->getRefCount() == 0)
    {
        world::entity *ety = entity->entity();
        std::string *name = (std::string*) ety->id();
        if (name != NULL)
        {
            NamedEntities.erase (*name);
        }
        
        entity_iter i = std::find (Entities.begin(), Entities.end(), ety);
        if (i != Entities.end())
        {
            Entities.erase (i);
        }
    }
}

// find all zones in the given view
std::list<world::zone*> MapData::zones_in_view (const s_int32 & x, const s_int32 & y, const s_int32 & z, const s_int32 & length, const s_int32 & width) const
{
    std::list<world::zone*> result;
    
    s_int32 max_x = x + length;
    s_int32 min_yz = y - z;
    s_int32 max_yz = min_yz + width;
    
    std::list<world::zone *>::const_iterator i;
    for (i = Zones.begin(); i != Zones.end(); i++)
    {
        // no overlap on x-axis
        if (max_x < (*i)->min().x() || x > (*i)->max().x()) continue;
        // no overlap on y/z-axis
        if (max_yz < ((*i)->min().y() - (*i)->max().z()) || min_yz > ((*i)->max().y() - (*i)->min().z())) continue;
        
        result.push_back (*i);        
    }

    return result;
}
