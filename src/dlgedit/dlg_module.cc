/*
   $Id: dlg_module.cc,v 1.3 2008/09/19 18:09:39 ksterker Exp $

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
 * @file dlg_module.cc
 *
 * @author Kai Sterker
 * @brief One complete dialogue or sub-dialogue.
 */

#include <algorithm>
#include "dlg_module.h"
#include "dlg_circle.h"
#include "dlg_arrow.h"
#include "gui_dlgedit.h"
#include "gui_resources.h"

// ctor
DlgModule::DlgModule (std::string p, std::string n, std::string u, std::string d)
{
    entry_.setDescription (d);
    uid_ = u;
    path_ = p + "/";
    name_ = n;
            
    init ();
}

// dtor
DlgModule::~DlgModule ()
{
    GuiDlgedit::window->tree ()->removeModule (this);
}

// initialize a newly constructed DlgModule
void DlgModule::init ()
{
    type_ = MODULE;
    mode_ = IDLE;
    state_ = IDLE;
    selected_ = NULL;
    highlighted_ = NULL;
    parent_ = NULL;
    changed_ = false;
    displayed_ = false;
    nid_ = 0;
    serial_ = 1;
}

// reset dialogue to initial state
void DlgModule::clear ()
{
    // delete all nodes
    while (!nodes.empty ()) deleteNode (nodes.front ());
    
    // clear custom code and such
    entry_.clear ();
    
    // reset all variables
    init ();
}

// calculate shape of sub-dialogue
void DlgModule::initShape (const DlgPoint &center)
{
    // calculate width of the module icon
    GdkFont *font = GuiResources::font ();
    int width = gdk_string_width (font, name ().c_str ()) + 10;
    
    // align module to the (imaginary) grid and set shape
    top_left = DlgPoint (center.x (), center.y ());
    top_left.move (-width/2 - (center.x () % CIRCLE_DIAMETER), -(center.y () % CIRCLE_DIAMETER));
    resize (width, 20); 
}

// select a given node
bool DlgModule::selectNode (DlgNode *node)
{
    // try to find the given node in our array of nodes
    std::vector<DlgNode*>::iterator i = find (nodes.begin(), nodes.end(), node);
    
    // if it is not there, return
    if (i == nodes.end ()) return false;
    
    // see if a node is already selected
    if (selected_ != NULL) return false;
    
    // remember which node is selected
    selected_ = node;
    
    // set the mode of dialogue and node
    state_ = NODE_SELECTED;
    node->setMode (NODE_SELECTED);

    return true;
}

// highlight the given node
DlgNode* DlgModule::highlightNode (DlgNode *node)
{
    // mark the new node as highlighted ...
    DlgNode *prev = highlighted_;
    highlighted_ = node;

    // ... and return the node highlighted before
    return prev;
}

// select the dialogue's root node.
bool DlgModule::selectRoot ()
{
    for (std::vector<DlgNode*>::iterator i=nodes.begin(); i != nodes.end(); i++)
        if ((*i)->type () != LINK)
            if ((*i)->prev (FIRST) == NULL)
                return selectNode (*i);
    
    return false;
}

// get the node at the given postion
DlgNode* DlgModule::getNode (DlgPoint &position)
{
    // find the node with the given pos in our array of nodes
    std::vector<DlgNode*>::iterator i;
    
    for (i = nodes.begin (); i != nodes.end (); i++)
        if (*(*i) == position) return *i;
    
    return NULL;
}

// get the node with the given module and node ids
DlgNode* DlgModule::getNode (int mid, int nid)
{
    // first, get the module
    DlgModule *module = getModule (mid);

    // then, get the node
    if (module) return module->getNode (nid);
    
    return NULL;
}

// get node with the given node id
DlgNode* DlgModule::getNode (int id)
{
    // find the node with the given pos in our array of nodes
    std::vector<DlgNode*>::iterator i;
    
    for (i = nodes.begin (); i != nodes.end (); i++)
        if ((*i)->node_id () == id && (*i)->type () != MODULE) 
            return *i;
    
    return NULL;
}

// get module with the given module id   
DlgModule* DlgModule::getModule (int id)
{
    if (id == nid_) return this;
    
    std::vector<DlgNode*>::iterator i;
    DlgModule *module;
    
    for (i = nodes.begin (); i != nodes.end (); i++)
        if ((*i)->type () == MODULE) 
        {
        	// FIXME shouldn't this be (*i)->getModule(id) ???
            module = getModule (id);
            if (module) return module;
        }
    
    // nothing found
    return NULL;    
}

// add a node to the dialogue
void DlgModule::addNode (DlgNode *node)
{
    nodes.push_back (node);    
}

// delete a node from the dialogue
bool DlgModule::deleteNode ()
{
    // if no node is selected, there's nothing to delete
    if (selected_ == NULL) return false;
    
    // otherwise, first deselect it
    DlgNode *node = deselectNode ();
    
    // actually delete it
    deleteNode (node);
    
    return true;
}

// delete the given node
void DlgModule::deleteNode (DlgNode *node)
{
    // if the node is a circle, also delete the attached arrows
    if (node->type () != LINK)
    {
        // delete all preceding arrows
        for (DlgNode *i = node->prev (FIRST); i != NULL; i = node->prev (FIRST))
        {
            nodes.erase (remove (nodes.begin (), nodes.end (), i), nodes.end ());       
            if (highlighted_ == i) highlighted_ = NULL;
            delete i;
        }
        
        // delete all following arrows
        for (DlgNode *i = node->next (FIRST); i != NULL; i = node->next (FIRST))
        {
            nodes.erase (remove (nodes.begin (), nodes.end (), i), nodes.end ());     
            if (highlighted_ == i) highlighted_ = NULL;
            delete i;
        }
    }

    // remove the node itself from the vector
    nodes.erase (remove (nodes.begin (), nodes.end (), node), nodes.end ());       
    if (highlighted_ == node) highlighted_ = NULL;
    delete node;
}

// deselect the selected node
DlgNode* DlgModule::deselectNode ()
{
    DlgNode *retval = selected_;
    
    // see if a node is selected at all
    if (selected_ == NULL) return NULL;

    // unselect node
    state_ = IDLE;
    selected_->setMode (IDLE);
    
    // nothing selected
    selected_ = NULL;
    
    // return the node for drawing
    return retval;
}

// indicate that this module has been modified by the user
void DlgModule::setChanged (bool changed)
{
     if (changed_ != changed)
     {
         changed_ = changed;

         // updated view
         GuiDlgedit::window->setChanged ();
     }
}

// draw the module
void DlgModule::draw (GdkPixmap *surface, DlgPoint &offset, GtkWidget *widget)
{
    // get the color for drawing the circle
    GdkGC *gc = GuiResources::getColor (mode_, type_);

    // offset circle
    DlgPoint position = topLeft ().offset (offset);
    DlgRect area (position, width () + 1, height () + 1);

    // draw everything to the surface
    gdk_draw_rectangle (surface, GuiResources::getColor (GC_WHITE), TRUE, position.x (), position.y (), width (), height ());
    gdk_draw_rectangle (surface, gc, FALSE, position.x (), position.y (), width (), height ());

    // get the font to draw name
    GdkFont *font = GuiResources::font ();

    // place text in module's center
    int x = position.x () + 5;
    int y = position.y () + (height () + gdk_string_height (font, name ().c_str ())) / 2;
    gdk_draw_string (surface, font, gc, x, y, name ().c_str ());

    // Update the drawing area
    update (widget, area);
}

// get toplevel module
DlgModule* DlgModule::toplevel ()
{
    DlgModule *toplevel = this;
    
    while (toplevel->parent () != NULL) toplevel = toplevel->parent ();
    
    return toplevel;    
}

// Get extension of the graph for proper displaying
void DlgModule::extension (int &min_x, int &max_x, int &y)
{
    y = 0;
    min_x = 0;
    max_x = 0;
    
    for (std::vector<DlgNode*>::iterator i=nodes.begin(); i != nodes.end(); i++)
        if ((*i)->type () != LINK)
        {
            if ((*i)->y () < y) y = (*i)->y ();

            if ((*i)->x () < min_x) min_x = (*i)->x ();
            else if ((*i)->x () > max_x) max_x = (*i)->x ();
        }
}

// load a dialogue from file
bool DlgModule::load ()
{
    int i = 1, id = 0, n;
    DlgCircle *circle;
    DlgArrow *arrow;
    std::string s;

    // load all nodes and toplevel items
    while (i)
    {
        switch (i = parse_dlgfile (s, n))
        {
            // load text node
            case LOAD_CIRCLE:
            {
                circle = new DlgCircle (nid_, id++);
                circle->load ();
                
                nodes.push_back (circle);

                break;
            }

            // load arrow node
            case LOAD_ARROW:
            {
                arrow = new DlgArrow;

                if (arrow->load (this))
                    nodes.push_back (arrow);

                break;
            }

            // load submodule node
            case LOAD_MODULE:
            {
                if (parse_dlgfile (s, n) == LOAD_STR)
                {
                    // get filename of submodule
                    std::string file = path_ + s;

                    // try to create the subdialogue
                    DlgModule *subdlg = GuiDlgedit::window->loadSubdialogue (file);

                    // if succeeded, read the final bits
                    if (subdlg)
                    {
                        subdlg->loadSubdialogue ();
                        subdlg->setParent (this);
                        
                        nodes.push_back (subdlg);
                    }
                    
                }
                                
                break;
            }
            
            case LOAD_PROJECT:
            {
                if (parse_dlgfile (s, n) == LOAD_STR) entry_.setProject (s);
                break;                
            }

            case LOAD_NOTE:
            {
                if (parse_dlgfile (s, n) == LOAD_STR) entry_.setDescription (s);
                break;
            }

            case LOAD_FUNC:
            {
                if (parse_dlgfile (s, n) == LOAD_STR) entry_.setMethods (s);
                break;
            }
            
            case LOAD_CTOR:
            {
                if (parse_dlgfile (s, n) == LOAD_STR) entry_.setCtor (s);
                break;
            }
            
            case LOAD_DTOR:
            {
                if (parse_dlgfile (s, n) == LOAD_STR) entry_.setDtor (s);
                break;
            }
            
            case LOAD_IMPORTS:
            {
                if (parse_dlgfile (s, n) == LOAD_STR) entry_.setImports (s);
                break;
            }

            case LOAD_RACE:
            {
                if (parse_dlgfile (s, n) == LOAD_NUM); //->myplayer->set_val ("race", n);
                break;
            }

            case LOAD_GENDER:
            {
                if (parse_dlgfile (s, n) == LOAD_NUM); //->myplayer->set_val ("gender", n);
                break;
            }

            case LOAD_NPC:
            {
                if (parse_dlgfile (s, n) == LOAD_STR);
                break;
            }

            case LOAD_ID:
            {
                if (parse_dlgfile (s, n) == LOAD_NUM) serial_ = n;
                break;
            }
            
            default: break;
        }
    }
    
    // set serial of version 1 dialogues
    if (serial_ < id) serial_ = id;
    
    return true;
}

// load sub-dialogue
void DlgModule::loadSubdialogue ()
{
    int i = 1, n;
    std::string s;

    while (i)
    {
        switch (i = parse_dlgfile (s, n))
        {
            // module loaded or EOF
            case LOAD_END:
                return;
            
            // get id of submodule
            case LOAD_ID:
            {
                if (parse_dlgfile (s, n) == LOAD_NUM) nid_ = n;
                break;
            }

            // get position of submodule
            case LOAD_POS:
            {
                int x, y;
                GdkFont *font = GuiResources::font ();
                int width = gdk_string_width (font, name ().c_str ()) + 10;
                if (parse_dlgfile (s, n) == LOAD_NUM) x = n;
                if (parse_dlgfile (s, n) == LOAD_NUM) y = n;

                top_left = DlgPoint (x, y);
                bottom_right = DlgPoint (x + width, y + 20);
            }

           default: break;
        }
    }               
}

// save dialogue to file
bool DlgModule::save (std::string &path, std::string &name)
{
    // update path and name
    path_ = path + "/";
    if (name_ != name)
    {
        name_ = name;
        changed_ = true;
    }
    
    // open file
    std::ofstream out (fullName ().c_str ());
    
    // opening failed for some reasons    
    if (!out) return false;

    // Write Header: Adonthell Dialogue System file version 2
    out << "# Dlgedit File Format 2\n#\n"
        << "# Produced by Adonthell Dlgedit v" << _VERSION_ << "\n"
        << "# (C) 2000/2001/2002/2003 The Adonthell Team & Kai Sterker\n#\n"
        << "# $I" << "d$\n\n"
        << "Note §" << entry_.description () << "§\n\n";

    // Node ID
    out << "Id " << serial_ << "\n";
    
    // Save settings and stuff
    if (entry_.project () != "none")
        out << "Proj §" << entry_.project () << "§\n";
    
    if (entry_.imports () != "")
        out << "Inc  §" << entry_.imports () << "§\n";
    
    if (entry_.ctor () != "")
        out << "Ctor §" << entry_.ctor () << "§\n";

    if (entry_.dtor () != "")
        out << "Dtor §" << entry_.dtor () << "§\n";
    
    if (entry_.methods () != "")
        out << "Func §" << entry_.methods () << "§\n";
    
    // Save Circles first, as arrows depend on them when loading later on
    for (std::vector<DlgNode*>::iterator i = nodes.begin (); i != nodes.end (); i++)
        if ((*i)->type () != LINK)
            (*i)->save (out);
                
    // Save Arrows
    for (std::vector<DlgNode*>::iterator i = nodes.begin (); i != nodes.end (); i++)
        if ((*i)->type () == LINK)
            (*i)->save (out);

    // mark dialogue as unchanged
    setChanged (false);
    
    return true;
}

// save a sub-module
void DlgModule::save (std::ofstream &file)
{
    // module's relative filename
    file << "\nModule " << relativeName () << "\n";
            
    // module's id
    file << "  Id " << nid_ << "\n";
        
    // module's position
    file << "  Pos " << x () << " " << y () << "\n";
            
    file << "End\n";
}

// return the module's path relative to its parent
std::string DlgModule::relativeName ()
{
    // no parent -> return absolute Name
    if (parent_ == NULL) return fullName ();
    
    std::string p_path = parent_->path ();  // parent path
    std::string m_path = path_;             // module path
    std::string r_path = "";                // module's path relative to parent
    
    unsigned long pos = 0;
    
    // find the part of the filename that matches
    while (pos < m_path.length () && pos < p_path.length () &&
        p_path[pos] == m_path[pos]) pos++;
    
    // complete module path matched
    if (pos == m_path.length ())
    {
        // so either both files are in the same path, or the sub-dialogue
        // is on a higher level
        while ((pos = p_path.find ('/', pos)) != p_path.npos) r_path += "../";
    }

    // complete parent path matched 
    else if (pos == p_path.length ())
    {
        // so the sub-dialogue is in a sub-directory
        r_path = m_path.substr (pos);
    }
    
    // none of the paths completely matched
    else
    {
        // sub-dialogue is in a different directory on a higher level
        unsigned long i = pos;
        while ((i = p_path.find ('/', i)) != p_path.npos) r_path += "../";
        r_path += m_path.substr (pos);
    }
    
    return r_path + name_ + FILE_EXT;
}
