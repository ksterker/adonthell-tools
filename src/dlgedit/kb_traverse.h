/*
   $Id: kb_traverse.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file kb_traverse.h
 *
 * @author Kai Sterker
 * @brief Graph traversal via keyboard
 */

#ifndef KB_TRAVERSE_H
#define KB_TRAVERSE_H

#include "dlg_node.h"

/**
 * This class implements the graph traversal strategy for keyboard users.
 * As a single node may have both multiple children and multiple parents,
 * this is not that easy. The current strategy is the following:
 *
 * - when a node is selected, LEFT and RIGHT iterate over a list of
 *   siblings of that node.
 * - pressing UP will move to the first parent of the selected node. The
 *   list of siblings will contain all the remainig parents.
 * - pressing DOWN will move to the first child of the selected node. The
 *   list of siblings will contain all the remaining children.
 * - pressing a key with no node selected will select the first node
 *   without parent. The list of siblings will contain all other nodes
 *   without parents.
 *
 * If you got a better solution, please tell me :).
 */
class KBTraverse
{
public:
    /**
     * Default constructor.
     */
    KBTraverse ();
    /**
     * Default destructor.
     */
    ~KBTraverse ();

    /**
     * @name Movement
     */
    //@{
    /**
     * Select previous node in list and return new selection.
     * @return pointer to current selection or \b NULL if nothing selected.
     */
    DlgNode* left ();
    /**
     * Select next node in list and return new selection.
     * @return pointer to current selection or \b NULL if nothing selected.
     */
    DlgNode* right ();
    /**
     * Select first parent of current selection. Update list with
     * this node's siblings.
     * @return pointer to current selection or \b NULL if nothing selected.
     */
    DlgNode* up ();
    /**
     * Select first child of current selection. Update list with
     * its siblings.
     * @return pointer to current selection or \b NULL if nothing selected.
     */
    DlgNode* down ();
    //@}

    /**
     * @name Selection
     */
    //@{
    /**
     * Get all parents of selected node. Adds them to the list
     * of siblings and selects the first of them.
     * @return <b>true</b> if a parent can be selected, <b>false</b> otherwise.
     */
    bool selectParents ();
    /**
     * Get all children of selected node. Adds them to the list
     * of siblings and selects the first of them.
     * @return <b>true</b> if a child can be selected, <b>false</b> otherwise.
     */
    bool selectChildren ();
    /**
     * Select the given node. Update list with this nodes siblings. Use this
     * method to keep the keyboard selection up-to-date when selecting  nodes
     * in a different way.
     * @param node to be selected
     * @return <b>true</b> if node has been selected, <b>false</b> otherwise.
     */
    bool select (DlgNode *node);
    /**
     * Select the first of the module's root nodes.
     * @param nodes vector with the module's nodes.
     * @return pointer to current selection or \b NULL if nothing selected.
     */
    DlgNode *selectRoot (std::vector<DlgNode*> *nodes);
    /**
     * Empty list of nodes and clear selection.
     */
    void clear ();
    //@}

private:
    /**
     * Add a node to the list of siblings. The nodes are sorted by their
     * position. The ones with the smallest y value come first; if nodes have
     * the same y value, then those with the smallest x value come first.
     * @param node the DlgNode to add.
     */
    void addSibling (DlgNode *node);

    std::list<DlgNode*> Siblings;           // Nodes reachable with left/right
    std::list<DlgNode*>::iterator Current;  // Selected node
};

#endif // KB_TRAVERSE_H
