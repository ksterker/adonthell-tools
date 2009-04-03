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
