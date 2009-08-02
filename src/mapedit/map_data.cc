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

// rename map entity
world::entity* MapData::renameEntity (MapEntity *entity, const std::string & id)
{
    // first, check if the new name is valid
    std::hash_map<std::string, world::named_entity*>::const_iterator result = NamedEntities.find (id);
    if (result != NamedEntities.end())
    {
        // TODO: display message in status bar
        return NULL;
    }
    
    world::named_entity *ety = (world::named_entity *) entity->entity();
    if (!ety->is_unique())
    {
        // create entity with new name and existing object
        add_entity (ety->get_object(), id);
    }
    else
    {
        // create a completely new entity
        add_entity (ety->get_object()->type(), id);
    }
    world::entity *new_entity = getNewestEntity();
    
    // now replace old entity in map itself
    world::chunk_info *ci = entity->getLocation();
    world::chunk_info new_ci (new_entity, ci->Min, ci->Max);
    remove (*ci);
    add (new_ci);
    
    // delete old entity
    NamedEntities.erase (*ety->id());
    for (std::vector<world::entity*>::iterator i = Entities.begin(); i != Entities.end(); i++)
    {
        if (*i == ety)
        {
            Entities.erase (i);
            delete *i;
            break;
        }
    }
    
    delete ety;
    return new_entity;
}
