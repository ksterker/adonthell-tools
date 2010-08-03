/*
   $Id: dlg_point.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file dlg_point.h
 *
 * @author Kai Sterker
 * @brief A wrapper around GdkPoint.
 */

#ifndef DLG_POINT_H
#define DLG_POINT_H

#include <gdk/gdktypes.h>

/**
 * The representation of a point, defined by its x and y coordinate. 
 * 
 * This class provides some abstraction from the GdkPoint type 
 * defined in GTK+, to remain as independent from the actual GUI 
 * toolkit as possible.
 */
class DlgPoint
{
public:
    DlgPoint () : x_(0), y_(0) { }
    DlgPoint (int x, int y) : x_(x), y_(y) { }
    DlgPoint (GdkPoint &point) : x_(point.x), y_(point.y) { }
    
    int x () const                  { return x_; }
    int y () const                  { return y_; }
    
    /**
     * Creates a new point, which is moved by the values of the given point.
     * @param p the distance to offset this point
     * @return this point, offset by the point p
     */
    DlgPoint offset (DlgPoint &p) const;
    /**
     * Creates a new point, which is moved by the given values.
     * @param x the offset in x direction
     * @param y the offset in y direction
     * @return this point, offset by the given values.
     */
    DlgPoint offset (int x, int y)  { return DlgPoint (x_ + x, y_ + y); }
    /**
     * Offset this point by the given values.
     * @param x the offset in x direction
     * @param y the offset in y direction
     */
    void move (DlgPoint &p)         { x_ += p.x (); y_ += p.y (); }
    /**
     * Offsets this point by the given values.
     * @param x the offset in x direction
     * @param y the offset in y direction
     */
    void move (int x, int y)        { x_ += x; y_ += y; }

    /**
     * Convert a DlgPoint into a GdkPoint.
     */
    operator GdkPoint();
    
    /**
     * Assign another point to this one
     */
    DlgPoint &operator= (const DlgPoint &p)  
    { 
        x_ = p.x (); 
        y_ = p.y (); 
        return *this;
    }

    /**
     * Equal operator
     */
    bool operator== (const DlgPoint &p)
    {
        return x_ == p.x() && y_ == p.y();
    }
    
    /**
     * Less Than operator
     */
    bool operator< (const DlgPoint &p)
    {
        if (x_ == p.x ()) return y_ < p.y ();
        else return x_ < p.x ();
    }
    
private:
    int x_;
    int y_;
};
 
#endif // DLG_POINT_H
