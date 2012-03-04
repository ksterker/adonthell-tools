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
 * @file /adonthell-tools/src/common/uid.cc/uid.cc
 *
 * @author Kai Sterker
 * @brief Create unique ids
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <stdint.h>
#include <glib.h>

#include "uid.h"

#undef get16bits

#if (defined(__GNUC__) && defined(__i386__))
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

// create uid
u_int32 uid::create (const u_int32 & counter)
{
    const char *g_host = g_get_host_name ();
    return SuperFastHash(g_host, strlen(g_host)) ^ counter;
}

// return uid as 8byte hex string
std::string uid::as_string(const u_int32 & uid)
{
    std::ostringstream result;
    result << (std::hex) << uid;

    return result.str();
}

// parse uid from hex string
u_int32 uid::from_string(const std::string & uid)
{
    uint32_t result;

    std::istringstream iss(uid);
    iss >> (std::hex) >> result;

    return result;
}

// return simple hash
u_int32 uid::hash (const std::string & source)
{
    return SuperFastHash(source.c_str(), source.length());
}

/*
 * Taken from http://www.azillionmonkeys.com/qed/hash.html
 * Licensed under LGPL 2.1
 */
u_int32 uid::SuperFastHash (const char *data, s_int32 len)
{
    u_int32 hash = len, tmp;
    s_int32 rem;

    if (len <= 0 || data == NULL) return 0;

    rem = len & 3;
    len >>= 2;

    /* Main loop */
    for (;len > 0; len--)
    {
        hash  += get16bits (data);
        tmp    = (get16bits (data+2) << 11) ^ hash;
        hash   = (hash << 16) ^ tmp;
        data  += 2*sizeof (uint16_t);
        hash  += hash >> 11;
    }

    /* Handle end cases */
    switch (rem)
    {
        case 3:
        {
            hash += get16bits (data);
            hash ^= hash << 16;
            hash ^= data[sizeof (uint16_t)] << 18;
            hash += hash >> 11;
            break;
        }
        case 2:
        {
            hash += get16bits (data);
            hash ^= hash << 11;
            hash += hash >> 17;
            break;
        }
        case 1:
        {
            hash += *data;
            hash ^= hash << 10;
            hash += hash >> 1;
            break;
        }
    }

    /* Force "avalanching" of final 127 bits */
    hash ^= hash << 3;
    hash += hash >> 5;
    hash ^= hash << 4;
    hash += hash >> 17;
    hash ^= hash << 25;
    hash += hash >> 6;

    return hash;
}
