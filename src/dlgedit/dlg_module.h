/*
   $Id: dlg_module.h,v 1.1 2004/07/25 15:52:22 ksterker Exp $

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
 * @file dlg_module.h
 *
 * @author Kai Sterker
 * @brief One complete dialogue or sub-dialogue.
 */

#ifndef DLG_MODULE_H
#define DLG_MODULE_H

#include "dlg_module_entry.h"
#include "kb_traverse.h"

/**
 * This class contains a dialogue, made up of elementary nodes and
 * (optionally) sub-dialogues. It is different from the other 
 * nodes as it contains a complete dialogue, but can also be part
 * of another dialogue.
 */
class DlgModule : public DlgNode
{
public:
    /**
     * Create a new, empty dialogue module.
     * @param p The full path to the file containing the module.
     * @param n The actual name without file extension.
     * @param u Unique id of the module.
     * @param d Description of the module.
     * @param id Serial number of the module.
     */
    DlgModule (std::string p, std::string n, std::string u, std::string d);

    /**
     * Delete a subdialogue.
     */
    ~DlgModule ();
    
    /**
     * Reset the dialogue to its state after construction. Deletes all
     * nodes and clears the DlgModuleEntry.
     */
    void clear ();
    
    /**
     * Set the node id of this module. The toplevel module will always
     * have the id 0. Sub-modules will get the next unused node id of
     * the top-level module. That means, sub-modules may end up with the
     * same id than circles of the same parent module. But this is not as
     * tragic as it might seem, as circles and modules are treated
     * differently when searching for a certain node.
     */
    void setID ()
    {
        int &id = toplevel ()->serial ();
        nid_ = id;
        id++;
    }
    
    /**
     * @name Module drawing
     */
    //@{
    /**
     * Calculate this module's shape. The shape is a rectangle centered
     * around the given DlgPoint. It is large enough to display the
     * module's name.
     * @param center The point to center the module shape around.
     */
    void initShape (const DlgPoint &center);
    
    /**
     * Draw this node to the given surface with the specified offset. 
     * The node's current mode determines the color thw node is drawn with. 
     * @param surface the cairo_surface_t to draw to
     * @param offset the DlgPoint to use as offset
     * @param widget the GtkWidget to update after drawing
     */
    void draw (cairo_surface_t *surface, DlgPoint &offset, GtkWidget *widget);
    //@}
    
    /**
     * @name Node handling
     */
    //@{
    /**
     * Add the given node to the dialogue.
     * @param node the DlgNode to add.
     */    
    void addNode (DlgNode *node);
    
    /**
     * Delete the currently selected node from the dialogue. If the node 
     * is a circle, also deletes all attached arrows.
     * @return \b true> if the node has been deleted, \b false otherwise.
     */
    bool deleteNode ();
    
    /**
     * Delete the given node from the dialogue.
     * @param node The node to delete.
     */
    void deleteNode (DlgNode *node);
    
    /**
     * Select a node from the list of nodes.
     * @param node The DlgNode to select.
     * @return \b true if the node has been selected, \b false otherwise.
     */
    bool selectNode (DlgNode *node);

    /**
     * Mark the given node as highlighted, to show that is under the cursor.
     * @param node the DlgNode to be highlighted
     * @return the the previously highlighted node, or \b NULL if no 
     *         node has been highlighted
     */
    DlgNode* highlightNode (DlgNode *node);
    
    /**
     * Select the root node of the dialogue.
     * @return \b true if the node has been selected, \b false otherwise.
     */
    bool selectRoot ();

    /** 
     * Deselect a previously selected node.
     * @return the previously selected DlgNode, or \b NULL if no 
     *         node has been deselected
     */
    DlgNode* deselectNode ();

    /**
     * Get the node at the given position.
     * @return the DlgNode at the positon, or \b NULL if there is none.
     */
    DlgNode* getNode (DlgPoint &point);
               
    /**
     * Get the node that is currently selected.
     * @return the DlgNode currently selected
     */
    DlgNode* selected ()                { return selected_; }
    //@}
    
    /**
     * @name Node retrieval
     */
    //@{
    /**
     * Get the node with the given module and node id. First locates
     * the module with given mid. Then locates the node with the given
     * nid within this module.
     * @param mid The node id of the module the node is located in.
     * @param nid The node id of the node to retrieve.
     * @return the DlgNode with that id, or \b NULL if there is none.
     */
    DlgNode* getNode (int mid, int nid);
    /**
     * Get the node with the given node id in the current module.
     * @param nid The node id of the node to retrieve.
     * @return the DlgNode with that id, or \b NULL if there is none.
     */
    DlgNode* getNode (int id);
    /**
     * Get the (sub-)module with the given node id in the current module.
     * @param nid The node id of the node to retrieve.
     * @return the DlgNode with that id, or \b NULL if there is none.
     */
    DlgModule* getModule (int id);
    /**
     * Return the first parent in the chain of parents, i.e. the
     * toplevel module.
     * @return the module in the chain of parents with no parent.
     */
    DlgModule *toplevel ();
    //@}

     /**
     * Get the extension of the module for centering in view.
     *
     * @param min_x will contain position of leftmost node
     * @param min_y will contain position of rightmost node
     * @param y will contain position of topmost node
     */
    void extension (int &min_x, int &max_x, int &y);

    /**
     * @name Loading / Saving
     */
    //@{
    /**
     * Init the Dialogue from a file
     * @return \b true if loading was successful, \b false otherwise.
     */
    bool load ();
    
    /**
     * Init a sub-dialogue from a file.
     */
    void loadSubdialogue ();
    
    /**
     * Save the Dialogue to a file
     * @param path full path of the dialogue.
     * @param name file name without extension.
     * @return \b true if saving was successful, \b false otherwise.
     */
    bool save (std::string &path, std::string &name);
    
    /**
     * Save module as sub-dialogue. In contrast to the method above,
     * this only saves a 'reference' to the module. It will have to be
     * restored from the actual source file. 
     *
     * In order to allow different users to work on the same dialogues,
     * the reference will be relative to CfgProject::BaseDir. That way,
     * a dialogue referencing sub-dialogues can be loaded even if it is
     * moved to a different location, provided the relative paths of
     * included modules have not changed.
     *
     * @param file an opened file.
     */
    void save (std::ofstream &file);
    //@}
   
    /**
     * @name Filename and Path retrieval
     */
    //@{
    /**
     * Get the name of this dialogue.
     * @return a reference to the dialogue's name.
     */
    std::string &name ()                { return name_; }
    /**
     * Get the name and id of this dialogue. To be used for window
     * captions and window menu.
     * @return string composed of name and unique id.
     */
    std::string shortName ()            { return name_ + uid_; }
    /**
     * Get the full path and filename of this dialogue. To be used when
     * saving the dialogue.
     * @return string composed of path, filename and file extension.
     */
    std::string fullName ()             { return path_ + name_ + FILE_EXT; }
    /**
     * Return the path and filename of this dialogue relative to its
     * parent.
     * @return location on disk relative to parent.
     */
    std::string relativeName ();
    /**
     * Get the module's path
     * @return the full path of the module
     */
    std::string path ()                 { return path_; }
    //@}
    
    /**
     * @name Member access
     */
    //@{
    /**
     * Get the list of nodes in this dialogue.
     * @return a reference to the list of nodes.
     */
    std::vector<DlgNode*> &getNodes ()  { return nodes; }
    
    /**
     * Get the current offset of this dialogue.
     * @return a reference to the dialogue's offset.
     */
    DlgPoint &offset ()                 { return offset_; }
    
    /**
     * Get the serial number for the next node that is
     * created. It is up to the caller of this function to
     * increase the number if neccessary.
     * @return Id to use for the next node.
     */
    int &serial ()                      { return serial_; }
    
    /**
     * Whether this (sub-)dialogue has been in view before switching
     * dialogues.
     * @return \b true if that is the case, \b false otherwise.
     */
    bool displayed ()                   { return displayed_; }
    /**
     * Mark this dialogue as displayed.
     * @param c Set to \b true to mark the dialogue as displayed.
     */
    void setDisplayed (bool displayed)  { displayed_ = displayed; }
    
    /**
     * Check whether this dialogue has been changed since it's been
     * opened or saved.
     * @return \b true if that is the case, \b false otherwise.
     */
    bool changed ()                     { return changed_; }
    /**
     * Mark this dialogue as changed.
     * @param c Set to \b true to mark the dialogue as changed.
     */
    void setChanged (bool c = true);
    
    /**
     * Retrieve the parent dialogue of this module. If the module is
     * a sub-dialogue, this will point to the dialogue that contains
     * the module. If it is the top-level dialogue, the parent will be
     * NULL.
     * @return parent of this dialogue, or \b NULL if it's a top-level dialogue 
     */
    DlgModule *parent ()                { return parent_; }
    /**
     * Set the parent of this dialogue.
     * @param parent The module containing this sub-dialogue
     */
    void setParent (DlgModule *parent)  { parent_ = parent; }
    
    /** 
     * Get the current state of the module, i.e. whether a node is selected,
     * highlighted, etc ... .
     * @return The module's state.
     */
    mode_type state ()                  { return state_; }
    /** 
     * Set the state mode of the module.
     * @param state The modules new state.
     */
    void setState (mode_type state)     { state_ = state; }
    
    /**
     * Get a pointer to the module entry
     * @return the DlgModuleEntry of this module
     */
    DlgModuleEntry *entry ()            { return &entry_; }
    
    /**
     * Get a pointer to the keyboard graph traversal helper. This keeps track
     * of the current selection's siblings. Users can navigate through them
     * via the arrow keys.
     * @return the KBTraverse of this module
     */
    KBTraverse *traverse ()             { return &traverse_; }
    //@}
    
protected:
    std::vector<DlgNode*> nodes;// all the nodes in this dialogue
    DlgNode *selected_;         // the node currently selected
    DlgNode *highlighted_;      // the node currently under the cursor
    DlgModule *parent_;         // parent of sub-dialogue
            
    DlgPoint offset_;           // The current offset in the graph view
    bool displayed_;            // Whether that (sub-)dialogue was in view
    bool changed_;              // Whether there were changes since saving
    int serial_;                // Id to use for the next new node

    mode_type state_;           // one of NONE, HILIGHTED, SELECTED

    std::string name_;          // Short (file-) name of the dialogue 
    std::string path_;          // Path of the dialogue
    std::string uid_;           // Unique number of the dialogue
    
    DlgModuleEntry entry_;      // further content of the dialogue

    KBTraverse traverse_;       // Keyboard graph traversal helper
    
private:
    void init ();               // initialize a newly constructed DlgModule
};

#endif // DLG_MODULE_H
