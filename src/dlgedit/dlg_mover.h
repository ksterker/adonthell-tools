/*
   $Id: dlg_mover.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file dlg_mover.h
 *
 * @author Kai Sterker
 * @brief A dummy node for drag'n dropping DlgArrows.
 */

#ifndef DLG_MOVER_H
#define DLG_MOVER_H

#include "dlg_arrow.h"


/**
 * Arrows have to be connected to two nodes, so to drag them from one
 * Circle to another, they have to be connected to an (invisble) mover.
 */
class DlgMover : public DlgNode
{
public:
    /**
     * Creates a new mover at the given position.
     * @param position The center of the new mover
     */
    DlgMover (DlgPoint &position);
    
    /**
     * Attach an arrow to this mover. The method checks whether either
     * the tip or the tail of the arrow overlap the mover and so decides
     * how the arrow is dragged.
     * @param arrow a DlgArrow to be dragged.
     * @return <b>true</b> if the mover is close enough to the arrows end,
     *      <b>false</b> otherwise.
     */
    bool attach (DlgArrow *arrow);
    
    /**
     * Drop the mover onto the given node. Connects the arrow we are 
     * moving to the node, if possible.
     * @param node A DlgNode or <b>NULL</b> if there is no node at the
     *      cursor position.
     */
    void drop (DlgNode *node);
    
private:
    int moving;                 // is arrow dragged by it's tip or tail?
    DlgNode *oldCircle;         // the node the arrow was attached to before
    
    enum { NONE, TIP, TAIL };
};

#endif // DLG_MOVER_H
