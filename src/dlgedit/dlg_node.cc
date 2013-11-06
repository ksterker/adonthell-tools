/*
   $Id: dlg_node.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file dlg_node.cc
 *
 * @author Kai Sterker
 * @brief Base class for the dialogue objects.
 */

#include <math.h>
#include "dlg_node.h"

// Coonstructor
DlgNode::DlgNode (node_type t, DlgRect area)
{
    type_ = t;
    index_ = -1;
    top_left = area.topLeft ();
    bottom_right = area.bottomRight ();
}

// Get the next node
DlgNode* DlgNode::next (query_type pos, int offset)
{
    if (next_.empty ()) return NULL;
    else return getNode (n, next_, pos, offset);
}

// Get the previous node
DlgNode* DlgNode::prev (query_type pos, int offset)
{
    if (prev_.empty ()) return NULL;
    else return getNode (p, prev_, pos, offset);
}

// Get a node from the specified list
DlgNode* DlgNode::getNode (std::list<DlgNode*>::iterator &it, 
        std::list<DlgNode*> &lst, query_type &pos, int &offset)
{ 
    switch (pos)
    {
        case FIRST:
            // point to the next element
            it = lst.begin ();
            offset -= 1;
            break;
        case NEXT:
            // nothing needs be done
            break;
        case LAST:
            // point to the last element
            it = lst.end ();
            offset = -offset;
            break;
        case PREV:
            // reverse searching
            offset = -offset;
            break;
        case CURRENT:
            if (it != lst.end ()) return *it;
            else return *(lst.begin ());
            break;
        default:
            return NULL;
    }
    
    // forward iteration
    if (offset >= 0)
    {
        for (int i = 0; i < offset && it != lst.end (); i++, it++);
        if (it != lst.end ()) return *it;
    }
    
    // backward iteration
    else
    {
        int i;
        for (i = offset; i < 0 && it != lst.begin (); i++, it--);
        if (i == 0) return *it;
    }
    
    return NULL;
}

// add node to list of followers
void DlgNode::addNext (DlgNode *node)
{
    std::list<DlgNode*>::iterator i = next_.begin ();
    DlgRect *cmp1, *cmp2 = node->type () == LINK ? node->next (FIRST) : node;
     
    // search the proper place for insertion
    while (i != next_.end ())
    {
        cmp1 = (*i)->type () == LINK ? (*i)->next (FIRST) : *i;
        if (*cmp2 < *cmp1) break;
        i++;
    }
        
    // insert
    next_.insert (i, node);
}

// add node to list of precedessors
void DlgNode::addPrev (DlgNode *node)
{
    std::list<DlgNode*>::iterator i = prev_.begin ();
    DlgRect *cmp1, *cmp2 = node->type () == LINK ? node->prev (FIRST) : node;
     
    // search the proper place for insertion
    while (i != prev_.end ())
    {
        cmp1 = (*i)->type () == LINK ? (*i)->prev (FIRST) : *i;
        if (*cmp2 < *cmp1) break;
        i++;
    }
    
    prev_.insert (i, node);
}

// remove a node from the list of followers
void DlgNode::removeNext (DlgNode *node)
{
    next_.remove (node);
    n = next_.begin ();
}

// remove a node from the list of precedessors
void DlgNode::removePrev (DlgNode *node)
{
    prev_.remove (node);
    p = prev_.begin ();
}

// draw the node (with a certain mode)
void DlgNode::draw (cairo_surface_t *surface, DlgPoint &offset, GtkWidget *widget, mode_type m)
{
    // change the mode temporarily
    mode_type oldMode = mode_;
    mode_ = m;
    
    // draw
    draw (surface, offset, widget);
    
    // restore old mode
    mode_ = oldMode; 
}

void DlgNode::drawPolygon (cairo_t *cr, const GdkColor *color, const bool & filled, const DlgPoint *points, const int &num_points)
{
    gdk_cairo_set_source_color (cr, color);
    cairo_set_line_width (cr, 0.6);
    cairo_move_to (cr, points[0].x(), points[0].y());
    for (int i = 1; i < num_points; i++)
    {
        cairo_line_to (cr, points[i].x(), points[i].y());
    }
    filled ? cairo_fill (cr) : cairo_stroke (cr);
}

void DlgNode::drawCircle (cairo_t *cr, const GdkColor *color, const bool & filled, const int & x, const int & y, const int & radius)
{
    gdk_cairo_set_source_color (cr, color);
    cairo_set_line_width (cr, 0.6);
    cairo_arc (cr, x, y, radius, 0, 2 * M_PI);
    filled ? cairo_fill (cr) : cairo_stroke (cr);
}

void DlgNode::drawRectangle (cairo_t *cr, const GdkColor *color, const bool & filled, const int & x, const int & y, const int & width, const int & height)
{
    gdk_cairo_set_source_color (cr, color);
    cairo_set_line_width (cr, 0.6);
    cairo_rectangle (cr, x, y, width, height);
    filled ? cairo_fill (cr) : cairo_stroke (cr);
}
