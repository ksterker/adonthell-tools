/*
   $Id: dlg_circle.h,v 1.1 2004/07/25 15:52:22 ksterker Exp $

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
 * @file dlg_circle.h
 *
 * @author Kai Sterker
 * @brief The node containing the actual dialogue text.
 */

#ifndef DLG_CIRCLE_H
#define DLG_CIRCLE_H

#include "dlg_node.h"
#include "dlg_circle_entry.h"

/**
 * This is the node that contains the actual dialogue text and associated
 * attributes, such as conditions or arbitrary (Python) code.
 *
 * There are three types of circles: Player, NPC and Narrator nodes.
 * - Player nodes contain text assigned to the person in front of the screen.
 * - NPC nodes are assigned to a certain, computer controlled entity.
 * - Narrator nodes are assigned to an (imaginary) voice from the off.  
 */
class DlgCircle : public DlgNode
{
public:
    /**
     * Create an empty circle. Used when loading a circle.
     * @param mid The id of the module the circle belongs to.
     * @param nid The id of the circle.
     */
    DlgCircle (int mid = 0, int nid = 0) 
    { mode_ = IDLE; entry_ = NULL; mid_ = mid; nid_ = nid; }

    /**
     * Create a circle with basic information. Used to create a new circle.
     * @param p The point where the circle needs to be created.
     * @param t The type of the circle (PLAYER, NPC or NARRATOR).
     * @param nid The unique id of the newly created circle.
     * @param mid The id of the module the circle belongs to.
     * @param e The actual contents of the circle.
     */
    DlgCircle (DlgPoint &p, node_type t, int nid, int mid, DlgCircleEntry *e = NULL);
    /**
     * Standard destructor.
     */
    ~DlgCircle ();
    
    /** 
     * @name Methods for graph traversal
     */
    //@{
    /**
     * This method retrieves a %DlgCircle preceeding this node, i.e.
     * (one of) it's parent(s).
     * @param pos Defines from what position to start retrieving the parent.
     * @param offset Get the parent found at the specified offset from the
     *        position given above.
     * @return The %DlgCircle found at the specified offset from the
     *        given position.
     * @see query_type
     */
    DlgCircle* parent (query_type pos, int offset = 1);
    /**
     * This method retrieves a %DlgCircle following this node, i.e.
     * (one of) it's child(ren). 
     * @param pos Defines from what position to start retrieving the child.
     * @param offset Get the child found at the specified offset from the
     *        position given above.
     * @return The %DlgCircle found at the specified offset from the 
     *        given position.
     * @see query_type
     */
    DlgCircle* child (query_type pos, int offset = 1);  
    //@}

    /**
     * load circle from a file. The file to load from is a global variable, 
     * as loading is handled by a scanner generated with flex.
     * @return <b>true</b> on success, <b>false</b> otherwise.
     */
    bool load ();
    
    /**
     * save a circle to a file
     * @param file Stream to an open file
     */
    void save (std::ofstream &file);

    /**
     * Draw this node to the given surface with the specified offset. 
     * The node's current mode determines the color thw node is drawn with. 
     * @param surface the GdkPixmap to draw to
     * @param offset the DlgPoint to use as offset
     * @param widget the GtkWidget to update after drawing
     */
    void draw (GdkPixmap *surface, DlgPoint &offset, GtkWidget *widget);

    /**
     * Get the contents of this node.
     * @return a DlgCircleEntry with the circle's text, code, conditions, etc.
     */
    DlgCircleEntry *entry ()            { return entry_; }
    
    /**
     * Get the text of this circle
     * @return a string of the form "<NPC>: <Text>" if a certain NPC is 
     *         attached to the node, otherwise returns only the text.
     */
    std::string text ();
    
    /**
     * Get the text of this circle for use with the GuiTooltip
     * @return a string of the form "<condition>\n<Text>\n<code>".
     */
    std::string tooltip ();

    /**
     * Check whether any arbitrary Python code is assigned to this circle.
     * @return <b>true</b> if this is the case, <b>false</b> otherwise.
     */
    bool hasCode ()                     { return entry_->hasCode (); }
    
    /**
     * Check whether the given node is a child of this circle
     * @return <b>true</b> if this is the case, <b>false</b> otherwise.
     */
    bool hasChild (DlgNode *child);
    
private:
    DlgCircleEntry *entry_;
};

#endif // DLG_CIRCLE_H
