/*
   $Id: dlg_rect.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file dlg_rect.cc
 *
 * @author Kai Sterker
 * @brief A wrapper around GdkRectangle.
 */

#include "dlg_rect.h"

// Cast GdkRectangle to DlgRect
DlgRect::DlgRect (GdkRectangle &rect)
{
    top_left = DlgPoint (rect.x, rect.y);
    bottom_right = DlgPoint (x () + rect.width, y () + rect.height);
}

// Cast DlgRect to GdkRectangle
DlgRect::operator GdkRectangle ()
{
    GdkRectangle rect;
    
    rect.x = x ();
    rect.y = y ();
    rect.width = width ();
    rect.height = height ();
    
    return rect;
}

// Create a valid rectangle from any two points
void DlgRect::init (DlgPoint &a, DlgPoint &b)
{
    top_left     = DlgPoint (a.x () < b.x () ? a.x () : b.x (), 
                             a.y () < b.y () ? a.y () : b.y ());
    bottom_right = DlgPoint (a.x () < b.x () ? b.x () : a.x (), 
                             a.y () < b.y () ? b.y () : a.y ());
}

// create a Rectangle from a point and width and height
DlgRect::DlgRect (DlgPoint &p, int width, int height)
{
    DlgPoint q (p.x () + width, p.y () + height);
    init (p, q);
}

// Test whether point lies within this rectangle's boundaries
bool DlgRect::contains (const DlgPoint &point)
{
    if (point.x () < x () || point.x () > bottom_right.x () ||
        point.y () < y () || point.y () > bottom_right.y ())
        return false;

    return true;
}

// Test whether the given rectangle overlaps with this one
bool DlgRect::contains (const DlgRect &rect)
{
    if (x () <= rect.x ()) {
        if (bottomRight ().x () >= rect.x ()) {
            if (y () <= rect.y ()) {
                if (bottomRight ().y () >= rect.y ()) return true;
            } else {
                if (rect.bottomRight ().y () >= y ()) return true;
            }    
        }
    } else {
        if (rect.bottomRight ().x () >= x ()) {
            if (y () <= rect.y ()) {
                if (bottomRight ().y () >= rect.y ()) return true;
            } else {
                if (rect.bottomRight ().y () >= y ()) return true;
            }    
        }
    }

    return false;
}

// Create a copy of this rectangle, inflated by the amount specified by p
DlgRect DlgRect::inflate (int x, int y)
{
    DlgPoint tl (top_left.x () - x, top_left.y () - y);
    DlgPoint br (bottom_right.x () + x, bottom_right.y () + y);
    
    return DlgRect (tl, br);
}

// Grow this rectangle
void DlgRect::grow (int x, int y)
{
    top_left = top_left.offset (-x, -y);
    bottom_right = bottom_right.offset (x, y);
}

// Move this rect to a new position
void DlgRect::setPos (const DlgPoint &pos)
{
    int w = width ();
    int h = height ();
    
    top_left = pos;
    resize (w, h);
}

// Move this rect by the given amount
void DlgRect::move (DlgPoint &p)
{
    top_left = DlgPoint (top_left.x () + p.x(), top_left.y() + p.y());
    bottom_right = DlgPoint (bottom_right.x () + p.x(), bottom_right.y() + p.y());
}

// Change the size of the rectangle.
void DlgRect::resize (int width, int height)
{
    bottom_right = DlgPoint (top_left.x () + width, top_left.y () + height);
}
    
// compare position of two rects 
bool DlgRect::operator< (DlgRect &rect)
{
    return center () < rect.center ();
}
