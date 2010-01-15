/*
 Copyright (C) 2009/2010 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file common/util.h
 *
 * @author Kai Sterker
 * @brief Helper methods.
 */

#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include <string>

/** 
 * Some helper methods used across the different tools.
 */
class util
{
public:
    /**
     * Create a path that is relative to either the user supplied or builtin
     * data directory. If the given path points into neither of those, try to
     * remove anything before the specified target_dir (so that the returned
     * relative path starts with target_dir).
     * @param path the path to make relative.
     * @param target_dir starting directory for a relative path used as fallback.
     * @return a relative path on success, or a canonical absolute path on error.
     */
    static std::string get_relative_path (const std::string & path, const std::string & target_dir);

private:
    /**
     * Try to create a path relative to the given base path.
     * This is used so we can store objects relative to the data directory.
     *
     * @note Note that path will be modified by the method to be a relative
     * path on success or a canonical absolute path on failure.
     *
     * @param path full path to make relative.
     * @param base_path path to the data directory.
     * @return true if base path has been subtracted from the path.
     */    
    static bool remove_common_path (std::string & path, const std::string & base_path);
};

#endif
