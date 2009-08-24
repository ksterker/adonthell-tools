/*
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file modeller/mdl_handle.h
 *
 * @author Kai Sterker
 * @brief Definitions for shape handles.
 */

#ifndef MDL_HANDLE_H
#define MDL_HANDLE_H

/// size of handle
#define HANDLE_SIZE 8
/// offset from handle center
#define HANDLE_OFFSET HANDLE_SIZE/2

/**
 * Handle types
 */
enum
{
    POSITION = 0,
    LENGTH = 1,
    WIDTH = 2,
    HEIGHT = 3,
    MAX_HANDLES = 4
};

#endif
