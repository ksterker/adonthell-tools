/*
   $Id: dlg_mover.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file dlg_mover.cc
 *
 * @author Kai Sterker
 * @brief A dummy node for drag'n dropping DlgArrows.
 */

#include "dlg_mover.h"

// create a new mover
DlgMover::DlgMover (DlgPoint &p)
{
    // Let position be the center of the mover
    top_left = DlgPoint (p.x () - 15, p.y () - 15);
    bottom_right = DlgPoint (x () + 30, y () + 30);
    
    type_ = MOVER;
    moving = NONE;
    oldCircle = NULL;
}

// attach an arrow to the mover
bool DlgMover::attach (DlgArrow *arrow)
{
    // Is arrow dragged by its tip?
    if (contains (arrow->getTip ()))
    {
        // save old Circle, in case we have to abort dragging
        oldCircle = arrow->next (FIRST);
        
        // remember what we are moving
        moving = TIP;

        // connect mover and arrow
        arrow->removeNext (oldCircle);
        arrow->addNext (this);
        
        // finally attach the arrow
        prev_.push_back (arrow);
        
        return true;
    }

    // arrow dragged by its tail?
    if (contains (arrow->getTail ()))
    {
        // save old Circle, in case we have to abort dragging
        oldCircle = arrow->prev (FIRST);
        
        // remember what we are moving
        moving = TAIL;

        // connect mover and arrow
        arrow->removePrev (oldCircle);
        arrow->addPrev (this);
        
        // finally attach the arrow
        next_.push_back (arrow);
        
        return true;
    }
    
    return false;
}

// drop the arrow onto the given node, if possible
void DlgMover::drop (DlgNode *node)
{
    // get the arrow attached to this mover
    DlgNode *arrow = (moving == TIP ? prev_.front () : next_.front ());
    DlgNode *circle;
    
    // test whether node is any good
    if (node == NULL || node->type () == LINK || 
        node == arrow->next (FIRST) || node == arrow->prev (FIRST))
        circle = oldCircle;
    else
        circle = node;
    
    // tail was moved
    if (moving == TAIL)
    {
        // do not link two player nodes
        if (circle->type () == PLAYER && arrow->next (FIRST)->type () == PLAYER)
            circle = oldCircle;

        // remove arrow from old circle
        oldCircle->removeNext (arrow);
        arrow->removePrev (this);
        
        // connect circle and arrow
        circle->addNext (arrow);
        arrow->addPrev (circle);

        // reorder precedessor of arrow
        arrow->next (FIRST)->removePrev (arrow);
        arrow->next (FIRST)->addPrev (arrow);
    }

    // tip was moved
    else
    {
        // do not link two player nodes
        if (circle->type () == PLAYER && arrow->prev (FIRST)->type () == PLAYER)
            circle = oldCircle;

        // remove arrow from old circle
        oldCircle->removePrev (arrow);
        arrow->removeNext (this);

        // connect circle and arrow
        circle->addPrev (arrow);
        arrow->addNext (circle);
        
        // reorder precedessor of arrow
        arrow->prev (FIRST)->removeNext (arrow);
        arrow->prev (FIRST)->addNext (arrow);
    }
    
    // redraw arrow
    ((DlgArrow *) arrow)->initShape ();
    
    return;
}
