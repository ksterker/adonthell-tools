/*
   $Id: dlg_rect.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file dlg_rect.h
 *
 * @author Kai Sterker
 * @brief A wrapper around GdkRectangle.
 */

#ifndef DLG_RECT_H
#define DLG_RECT_H

#include "dlg_point.h"

/**
 * The representation of a rectangle, defined by its upper left, and
 * bottom right corners. 
 * 
 * This class provides an abstraction from the GdkRectangle type 
 * defined in GTK+, to remain as independent from the actual GUI 
 * toolkit as possible.
 *
 * Moreover, it extends the functionality of GdkRectangle by providing
 * various query and comparison operations.
 */
class DlgRect
{
 public:
    /**
     * Default-constructor.
     */
    DlgRect () { }
    /**
     * Cast a GdkRectangle to a %DlgRect.
     * @param rect The GdkRectangle to convert.
     */
    DlgRect (GdkRectangle &rect);
    /**
     * Create a rectangle from two points. No matter where point a is
     * located relative to point b, the resulting rectangle will always
     * be valid (i.e. it's top left corner will be left, and above from
     * the bottom right corner).
     * @param a The first point.
     * @param b The second point. 
     */
    DlgRect (DlgPoint &a, DlgPoint &b)      { init (a, b); } 
    /**
     * Create a rectangle from a given origin and it's width and height.
     * @param p The origin (top, left corner) of the new rectangle
     * @param width the width
     * @param height the height
     */
    DlgRect (DlgPoint &p, int width, int height);

    /**
     * @name Queries for Attributes
     */ 
    //@{
    /**
     * Retrieve the absolute position of the rectangle's top left corner.
     * @return a reference to the top left corner.
     */
    const DlgPoint &topLeft () const        { return top_left; }
    /**
     * Retrieve the the absolute position of the rectangle's bottom right corner.
     * @return a reference to bottom right corner.
     */
    const DlgPoint &bottomRight () const    { return bottom_right; }
    /**
     * Retrieve the center of the rectangle.
     * @return the absolute position of the rectangle's center.
     */
    DlgPoint center () const                { return DlgPoint (x () + width ()/2, y () + height ()/2); }
    
    /**
     * Retrieve the rectangle's x coordinate, i.e. the absolute x value
     * of it's top left corner.
     * @return the x value of the top left corner.
     */
    int x () const                          { return top_left.x (); }
    /**
     * Retrieve the rectangle's y coordinate, i.e. the absolute y value
     * of it's top left corner.
     * @return the y value of the top left corner.
     */
    int y () const                          { return top_left.y (); }
    /**
     * Retrieve the rectangle's width, i.e. the distance between it's 
     * left and right side.
     * @return the width of the rectangle.
     */
    int width () const                      { return bottom_right.x () - x (); }
    /**
     * Retrieve the rectangle's height, i.e. the distance between it's 
     * top and bottom side.
     * @return the height of the rectangle.
     */
    int height () const                     { return bottom_right.y () - y (); }
    //@}

    /**
     * @name Various geometrical Tests
     */ 
    //@{
    /**
     * Test whether a given point lies within the rectangle's boundaries.
     * @param point The point to test against.
     * @return <b>true</b> when the point lies inside the rectangle, 
     *         <b>false</b> otherwise.
     */
    bool contains (const DlgPoint &point);

    /**
     * Test whether a given rectangle lies completely within this rectangle's
     * boundaries, or overlaps it in some fashion.
     * @param rect The rectangle to test against.
     * @return <b>true</b> when rect lies (partly) within the rectangle's
     *         boundaries, <b>false</b> otherwise.
     */
    bool contains (const DlgRect &rect);
    //@}

    /**
     * Make a copy of this rectangled, inflated by the specified amount.
     * @param x Value to inflate in x dimensions.
     * @param y Value to inflate the y dimensions.
     * @return A new, inflated rectangle with the same center as this one. 
     */
    DlgRect inflate (int x, int y);
    
    /**
     * Grow all four sides of this rectangle.
     * @param x Value to grow in x dimensions.
     * @param y Value to grow in y dimensions.
     * @return A new, inflated rectangle with the same center as this one. 
     */
    void grow (int x, int y);

    /**
     * Move the rectangle to a new position.
     * @param pos The new position of the rectangle
     */
    void setPos (const DlgPoint &pos);
        
    /**
     * Move this rectangle by the specified amount.
     * @param p Point representing the offset this rectangle shall be moved
     */
    void move (DlgPoint &p);
         
    /**
     * Change the size of the rectangle.
     * @param widht The new width.
     * @param height The new height.
     */
    void resize (int width, int height);
    
    /**
     * Convert a DlgRect into a GdkRectangle.
     */
    operator GdkRectangle();
    
    /**
     * Compare the positions of two rectangles. If they have the same y
     * value, the x value determines the result, otherwise the y value.
     * @param rect The rectangle to compare with.
     * @return <b>true</b> if the given rectangle is smaller than this one,
     *         <b>false</b> otherwise.
     */
    bool operator< (DlgRect &rect);
    
protected:
    DlgPoint top_left;
    DlgPoint bottom_right;
    
    void init (DlgPoint &a, DlgPoint &b);
};

#endif // DLG_RECT_H
