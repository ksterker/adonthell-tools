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
 * @file common/util.cc
 *
 * @author Kai Sterker
 * @brief Helper methods.
 */

#include <sys/param.h>
#include <stdlib.h>

#ifdef WIN32
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#undef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif

#include <base/base.h>

#include "util.h"

// get path relative to data directory
std::string util::get_relative_path (const std::string & path, const std::string & target_dir)
{
    std::string base_path = MK_UNIX_PATH (base::Paths.user_data_dir());
    std::string rel_path = path;
    
    // make sure to use path relative to (user defined) data directory
    if (base_path == "" || !remove_common_path (rel_path, base_path))
    {
        // fallback to builtin data dir if that doesn't seem to work
        base_path = MK_UNIX_PATH (base::Paths.game_data_dir());
        if (!remove_common_path (rel_path, base_path))
        {
            // if everythin fails, try locating target_dir in the path and use 
            // that as relative path
            size_t pos = rel_path.rfind (target_dir);
            if (pos != std::string::npos)
            {
                rel_path = rel_path.substr (pos);
            }
        }
    }
    
    return rel_path;
}

// try to make path relative to base path
bool util::remove_common_path (std::string & path, const std::string & base_path)
{
    // make canonical base path
    char canonical_path[PATH_MAX];
    if (realpath(base_path.c_str(), canonical_path))
    {
        std::string c_base_path = canonical_path;
        if (realpath(path.c_str(), canonical_path))
        {
            path = MK_UNIX_PATH (canonical_path);
            if (path.compare (0, c_base_path.size(), c_base_path) == 0)
            {
                path = path.substr (c_base_path.length());
                if (path[0] == '/')
                {
                    path = path.substr (1);
                }
                return true;
            }
        }
    }
    
    return false;
}

// convert windows directory separators to unix style
std::string util::to_unix_path (const std::string & path)
{
	std::string result = path;
	for (std::string::iterator i = result.begin(); i != result.end(); i++)
	{
		if (*i == '\\') *i = '/';
	}
	return result;
}
