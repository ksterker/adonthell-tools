/*
   $Id: gui_tree.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file gui_tree.h
 *
 * @author Kai Sterker
 * @brief View for the dialogue structure
 */

#ifndef GUI_TREE_H
#define GUI_TREE_H

#include <gtk/gtkctree.h>
#include "dlg_module.h"

/**
 * The %GuiTree displays the structure of a dialogue. Its root
 * is the top-level dialogue, its children are the various
 * sub-dialogues.
 *
 * The current selection of the tree will always match the
 * dialogue currently displayed by the graph view. Clicking on
 * an entry will have the same effect as clicking on a 
 * sub-dialogue node the graph view, i.e. switch to that 
 * module. 
 */
class GuiTree
{
public:
    /**
     * Speaking names for node icons,
     */    
    enum {
        BUBBLE          = 0,
        BUBBLE_SEL      = 1,
        BUBBLE_MOD      = 2,
        BUBBLE_SEL_MOD  = 3,
        PROJECT         = 4, 
        MAX_ICONS       = 5
    };
    
    /**
     * Constructor. Creates the tree widget and adds it to the
     * given pane.
     * @param paned The GtkPaned that will hold the tree widget. 
     */
    GuiTree (GtkWidget* paned);
    /**
     * Standard desctructor.
     */
    ~GuiTree ();
    
    /**
     * Displays the given module in the tree. In case the module
     * is no top-level dialogue, this method will search backwards
     * through the list of parents until it finds the top-level
     * dialogue. It will then display the structure of this dialogue.
     *
     * @param module A module whose structure should be displayed.
     */
    void addModule (DlgModule *module);
    /**
     * Refresh the given module. This will remove the module sub-tree
     * and generate it anew. To be called after reverting to saved.
     *
     * @param module The module to refresh.
     */
    void updateModule (DlgModule *module);
    /**
     * Attach the given module to a new project. To be called after 
     * changine the project the module belongs to.
     *
     * @param module The module to move to new project.
     */
    void updateProject (DlgModule *module);
    /**
     * Remove a module sub-tree. To be called when a module is closed
     * or a sub-module is deleted.
     *
     * @param The module to remove.
     */
    void removeModule (DlgModule *module);
    
    /**
     * Add available projects to the tree.
     */
    void addProjects ();
    
    /**
     * Empty the widget.
     */
    void clear ();
    
    /**
     * Update the state (saved/modified) of the module's icon.
     * @param module Module whose state (and icon) to update
     */
    void setChanged (DlgModule *module);
    /**
     * Update the name of the given module.
     * @param module Module whose name should be updated.
     */
    void setName (DlgModule *module);
    
    /**
     * Insert a module into the tree, as child of the given parent.
     * @param parent The parent of the module to add.
     * @param module The module to add.
     */
    void insert (DlgModule *parent, DlgModule *module);

    /**
     * Select the given node of the tree. If another node is selected,
     * it will be deselected first.
     * @param node The node to be selected.
     */
    void select (GtkCTreeNode *node);
    /**
     * Select the given module in the tree. If a node is currently
     * selected, it will be deselected first.
     * @param module The module to select.
     */
    void select (DlgModule *module);
    
private:
    /**
     * Build the initial module tree, by descending recursively
     * into the module's sub-dialogues, sub-sub-dialogues, and so on.
     * @param root The root of the (sub-)tree to be build.
     */
    void build (GtkCTreeNode *root);
    /**
     * Insert a module into the tree, as child of the given parent.
     * @param parent The parent of the module to add.
     * @param sibling The sibling after which module should be added.
     * @param module The module to add.
     * @return The node that has been added.
     */
    GtkCTreeNode *insert (GtkCTreeNode *parent, GtkCTreeNode *sibling, DlgModule *module);
    /**
     * Set the icon of the given node to the appropriate state.
     * @param node Node whose icon to change.
     * @param select Whether to use the selected or normal icon.
     * @param changed Whether the given node is saved or modified.
     */
    void setIcon (GtkCTreeNode *node, bool select, bool changed);
    /**
     * Find the node associated with the given module.
     * @param module DlgModule whose node to retrieve.
     * @return Node associated with the module, or NULL if it isn't found.
     */
    GtkCTreeNode *locate (DlgModule *module);
    /**
     * Find the node associated with the given project. If no such node
     * exists, it will be created.
     * @param project Name of the project
     * @return Node associated with project
     */
    GtkCTreeNode *locateProject (const std::string &project);
    
    GtkWidget *tree;        // The actual GTK+ tree widget
    GtkCTreeNode *selected; // The node currently 'selected'
    
    // Icons to display next to tree nodes
    GdkPixmap *icon[MAX_ICONS];
    GdkBitmap *mask[MAX_ICONS];
};

#endif // GUI_TREE_H
