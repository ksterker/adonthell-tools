/*
   $Id: dlg_point.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file dlg_point.cc
 *
 * @author Kai Sterker
 * @brief A wrapper around GdkPoint.
 */

#include "dlg_point.h"

// Cast DlgRect to GdkRectangle
DlgPoint::operator GdkPoint ()
{
    GdkPoint point;
    
    point.x = x ();
    point.y = y ();
    
    return point;
}

// move the point by a certain offset
DlgPoint DlgPoint::offset (DlgPoint &p) const
{
    return DlgPoint (x_ + p.x (), y_ + p.y ());
}
