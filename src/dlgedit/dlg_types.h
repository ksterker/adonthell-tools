/*
   $Id: dlg_types.h,v 1.2 2004/08/02 07:39:24 ksterker Exp $

   Copyright (C) 2002 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Dlgedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dlgedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file dlg_types.h
 *
 * @author Kai Sterker
 * @brief Typedefs and Enumerations used throughout dlgedit
 */

#ifndef DLG_TYPES_H
#define DLG_TYPES_H

/**
 * The type of a given DlgNode.
 */
enum node_type
{
    PLAYER          = 1,
    NPC             = 2,
    LINK            = 3,
    MOVER           = 4,
    NARRATOR        = 5,
    MODULE          = 6
};

/**
 * Specifies which node to retrieve.
 */ 
enum query_type
{
    FIRST           = 0,
    NEXT            = 1,
    PREV            = 2,
    LAST            = 3,
    CURRENT         = 4
};

/**
 * Defines the state of an individual node, but also that of the whole program.
 */
enum mode_type
{
    IDLE            = 0,
    NODE_SELECTED   = 1,
    NODE_HILIGHTED  = 2,
    NODE_DRAGGED    = 3,
    L10N_PREVIEW    = 4,
    NUM_MODES       = 5
};

/**
 * A few predefined colors for drawing operations
 */
enum
{
    GC_BLACK        = 0,
    GC_GREY         = 1,
    GC_WHITE        = 2,
    GC_DARK_RED     = 3,
    GC_RED          = 4,
    GC_DARK_GREEN   = 5,
    GC_GREEN        = 6,
    GC_ORANGE       = 7,
    GC_DARK_BLUE    = 8,
    GC_BLUE         = 9,
    MAX_GC          = 10
};

/**
 * Talking names for the different file menu items.
 */
enum menu_item
{
    SAVE            = 0,
    SAVE_AS         = 1,
    CLOSE           = 2,
    SETTINGS        = 3,
    FUNCTIONS       = 4,
    COMPILE         = 5,
    PREVIEW         = 6,
    RUN             = 7,
    OPEN_RECENT     = 8,
    REVERT          = 9,
    MAX_ITEM        = 10
};

/**
 * Defines for the various parts of a dialogue source file.
 */
enum
{
    LOAD_CIRCLE     = 1,
    LOAD_ARROW      = 2,
    LOAD_END        = 3,
    LOAD_TYPE       = 4,
    LOAD_PREV       = 5,
    LOAD_NEXT       = 6,
    LOAD_LINK       = 7,
    LOAD_POS        = 8,
    LOAD_NOTE       = 9,
    LOAD_TEXT       = 10,
    LOAD_COND       = 11,
    LOAD_VARS       = 12,
    LOAD_FUNC       = 13,
    LOAD_ACT        = 14,
    LOAD_NAME       = 15,
    LOAD_RACE       = 16,
    LOAD_GENDER     = 17,
    LOAD_NPC        = 18,
    LOAD_STR        = 19,
    LOAD_NUM        = 20,
    LOAD_IMPORTS    = 21,
    LOAD_CTOR       = 22,
    LOAD_DTOR       = 23,
    LOAD_PROJECT    = 24,
    LOAD_LOOP       = 25,
    LOAD_FILE       = 26,
    LOAD_BASE_DIR   = 27,
    LOAD_MODULE     = 28,
    LOAD_ID         = 29,
    LOAD_UNKNOWN    = 30
};

/**
 * The size of a DlgCircle.
 */
#define CIRCLE_DIAMETER 20

/**
 * File extension of dialogue source files.
 */
#define FILE_EXT ".adlg"

#endif // DLG_TYPES_H
