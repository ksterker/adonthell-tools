/*
   $Id: dlg_node.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

   Copyright (C) 2002/2003 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file dlg_node.h
 *
 * @author Kai Sterker
 * @brief Base class for the dialogue objects.
 */

#ifndef DLG_NODE_H
#define DLG_NODE_H

#include <list>
#include <string>
#include <stdio.h>
#include <fstream>
#include "dlg_node_gfx.h"

/*
 * Function to read the next token from dialogue source file
 * @param str used to return string values
 * @param i used to return numeric values
 * @return Type of token read as defined in dlg_types.h
 */
extern int parse_dlgfile (std::string &str, int &i);

/**
 * File pointer for dialogue source files being parsed.
 */
extern FILE *loadlgin;


/**
 * This is the base class for all dialogue objects. As this, it has three
 * major purposes:
 *
 * - provide means for a graphical representation of the node in the Graph
 *   window. This functionality is inherited from the DlgNodeGfx class.
 *
 * - provide the glue between different Nodes, thus forming the actual graph.
 *
 * - provide a unique interface to the different flavours of Nodes in the
 *   graph.
 */
class DlgNode : public DlgNodeGfx
{
public:
    DlgNode () { index_ = -1; }
    DlgNode (node_type t, DlgRect area);

    /** 
     * @name Methods for graph traversal
     */
    //@{
    /**
     * Get a node from this node's list of direct followers.
     * @param pos Defines what to do with the iterator of that list.
     * @param offset Move the iterator by that offset.
     * @return The node found at the specified offset from the given
     *         iterator position.
     * @see query_type
     */
    DlgNode* next (query_type pos, int offset = 1);
    /**
     * Get a node from this node's list of direct precedessors.
     * @param pos Defines what to do with the iterator of that list.
     * @param offset Move the iterator by that offset.
     * @return The node found at the specified offset from the given
     *         iterator position.
     * @see query_type
     */
    DlgNode* prev (query_type pos, int offset = 1);  
    //@}

    /**
     * @name Node insertion
     */
    //@{
    /**
     * Add a node to the list of precedessors. The nodes are sorted by their
     * position. The ones with the smallest y value come first; if nodes have
     * the same y value, then those with the smallest x value come first. 
     * @param node the DlgNode to add.
     */
    void addPrev (DlgNode *node);
    /**
     * Add a node to the list of followers.
     * @param node the DlgNode to add.
     */
    void addNext (DlgNode *node);
    //@}
    
    /**
     * @name Node removal
     */
    //@{
    /**
     * Remove a node from the list of precedessors.
     * @param node the DlgNode to remove.
     */
    void removePrev (DlgNode *node);
    /**
     * Remove a node from the list of followers.
     * @param node the DlgNode to remove.
     */
    void removeNext (DlgNode *node);
    //@}

    /**
     * Draw this node to the given surface. During drawing, the mode is
     * temporarily changed to the given mode.
     * @param surface the GdkPixmap to draw to
     * @param offset the DlgPoint to use as offset
     * @param widget the GtkWidget to update after drawing
     * @param mode the new mode of the node
     */
    void draw (GdkPixmap *surface, DlgPoint &offset, GtkWidget *widget, mode_type mode);
    
    /**
     * Draw this node to the given surface.
     * @param surface the GdkPixmap to draw to
     * @param offset the DlgPoint to use as offset
     * @param widget the GtkWidget to update after drawing
     */
    virtual void draw (GdkPixmap *surface, DlgPoint &offset, GtkWidget *widget)
    { 
    }
    
    /**
     * Save a node to file
     * @param out Stream to the file to save to
     */
    virtual void save (std::ofstream &out) { }
    
    /**
     * Get the type of the node.
     * @return the type of the node.
     */
    node_type &type ()      { return type_; }
    
    /**
     * Return the node's unique id. This is given when the node is
     * created and will never change from then on. It can be used
     * to correctly identify nodes of subdialogues and the like.
     * @return unique id of the node.
     */
    int node_id ()          { return nid_; }
    
    /**
     * Returns id of the module this node belongs to. It allows us 
     * to tell nodes of different modules apart. Together with the
     * node id, this gives each node a unique id.
     * @return module id.
     */
    int module_id ()        { return mid_; }
    
    /**
     * Get the index of this node. Used by DlgCompile.
     * @return the node's index.
     */   
    int index ()            { return index_; }
    
    /**
     * Set the index of this node. Used by DlgCompile.
     * @param i the index to use for this node.
     */
    void setIndex (int i)   { index_ = i; }
        
protected:
    node_type type_;                    // type of the node
    int index_;                         // used by DlgCompiler
    int nid_;                           // unique id of the node
    int mid_;                           // unique id of module node belongs to
    
    std::list<DlgNode*> prev_;          // list of node's parents
    std::list<DlgNode*> next_;          // list of node's children

    std::list<DlgNode*>::iterator p;    // iterator over parents
    std::list<DlgNode*>::iterator n;    // iterator over children
    
    DlgNode* getNode (std::list<DlgNode*>::iterator &it, 
        std::list<DlgNode*> &lst, query_type &pos, int &offset);

    void drawPolygon (cairo_t *cr, const GdkColor *color, const bool & filled, const DlgPoint *points, const int &num_points);
    void drawCircle (cairo_t *cr, const GdkColor *color, const bool & filled, const int & x, const int & y, const int & radius);
    void drawRectangle (cairo_t *cr, const GdkColor *color, const bool & filled, const int & x, const int & y, const int & width, const int & height);
};

#endif // DLG_NODE_H
