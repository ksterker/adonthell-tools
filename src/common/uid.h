/*
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/** 
 * @file /adonthell-tools/src/common/uid.h/uid.h
 *
 * @author Kai Sterker
 * @brief Create unique ids
 */

#ifndef COMMON_UID_H
#define COMMON_UID_H

#include <string>
#include <adonthell/base/types.h>

/**
 * Class to create unique ids for various purposes. In general,
 * it is meant to allow distributed creation and editing of
 * map objects and such, where simple counters will not work if
 * two or more developers work on the same map or create models
 * in the same directory. So the idea is to enrich the counter
 * with information that is hopefully unique to each developer,
 * so the objects they create will not interfere.
 *
 * The ids generated are limited to 32bits, so collisions are
 * likely to occur eventually. These will have to be resolved
 * manually by changing the uid of the offending objects.
 */
class uid
{
public:
    /**
     * Create a unique id for the host and given counter.
     * This will create the same id for identical host and
     * counter pairs, but very likely different ids for
     * different host/counter pairs. YMMV.
     *
     * @param counter an integer.
     * @return probably a unique id.
     */
    static u_int32 create (const u_int32 & counter);

    /**
     * Convert 32 bit hash to 8 byte hex string.
     * @return hash in hex representation.
     */
    static std::string as_string (const u_int32 & uid);

    /**
     * Convert 8 byte hex string to 32bit hash.
     * @return hash in decimal representation.
     */
    static u_int32 from_string (const std::string & uid);

    /**
     * Create a hash from the given source. For the same source
     * the same hash is returned.
     * @return hash for given string.
     */
    static u_int32 hash (const std::string & source);

private:
    /**
     * Forbid construction.
     */
    uid();

    /**
     * Create a hash value from the given input string
     * @param data input string to hash
     * @param len size of the input string
     * @return the hash value for that string.
     */
    static u_int32 SuperFastHash (const char *data, s_int32 len);
};

#endif /* UID_H_ */
