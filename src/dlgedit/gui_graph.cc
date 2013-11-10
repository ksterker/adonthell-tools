/*
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
 * @file gui_graph.cc
 *
 * @author Kai Sterker
 * @brief View for the dialogue graph
 */

#include <gtk/gtk.h>
#include "cfg_data.h"
#include "dlg_mover.h"
#include "gui_dlgedit.h"
#include "gui_graph_events.h"
#include "gui_resources.h"
#include "gui_circle.h"
#include "gui_file.h"

// Constructor
GuiGraph::GuiGraph (GtkWidget *paned) : Scrollable ()
{
    // initialize members to sane values
    mover = NULL;
    module = NULL;
    offset = NULL;
    surface = NULL;
    tooltip = NULL;
    
    // create drawing area for the graph
    graph = gtk_drawing_area_new ();
    gtk_widget_set_size_request (graph, 200, 450);
    gtk_paned_add2 (GTK_PANED (paned), graph);
    gtk_widget_show (graph);
    gtk_widget_grab_focus (graph);
    
    // register our event callbacks
    g_signal_connect (G_OBJECT (graph), "expose_event", G_CALLBACK(expose_event), this);
    g_signal_connect (G_OBJECT (graph), "configure_event", G_CALLBACK(configure_event), this);
    g_signal_connect (G_OBJECT (graph), "button_press_event", G_CALLBACK(button_press_event), this);
    g_signal_connect (G_OBJECT (graph), "button_release_event", G_CALLBACK(button_release_event), this);
    g_signal_connect (G_OBJECT (graph), "motion_notify_event", G_CALLBACK(motion_notify_event), this);
    g_signal_connect (G_OBJECT (GuiDlgedit::window->getWindow ()), "key_press_event", G_CALLBACK(key_press_notify_event), this);
    
    gtk_widget_set_events (graph, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
        GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK);
}

// dtor
GuiGraph::~GuiGraph()
{
    cairo_surface_destroy(surface);
}

// attach a module
void GuiGraph::attachModule (DlgModule *m, bool cntr)
{
    module = m;
    
    // get reference of the module's offset
    offset = &m->offset ();
    
    // center the module in view
    if (cntr) center ();    

    // if a node is selected, update the instant preview
    GuiDlgedit::window->list ()->display (module->selected ());
    
    // update the module structure
    GuiDlgedit::window->tree ()->select (module);

    // update the program state
    GuiDlgedit::window->setMode (module->state ());

    GtkAllocation allocation;
    gtk_widget_get_allocation (graph, &allocation);

    // set the size of the dialogue
    drawing_area.resize (allocation.width, allocation.height);

    // tell the module that it is in view
    module->setDisplayed (true);
    
    // display the module
    draw ();
}

// detach a module
void GuiGraph::detachModule ()
{
    module = NULL;
    
    // clear the instant preview
    GuiDlgedit::window->list ()->clear ();
    
    // update the program state
    GuiDlgedit::window->setMode (IDLE);

    // remove the tooltip if it is open
    if (tooltip)
    {
        delete tooltip;
        tooltip = NULL;
    }
}

// display a different module
void GuiGraph::switchModule (DlgModule *m)
{
    // we're switching between sub-dialogues
    if (module) module->setDisplayed (false);
    
    detachModule ();
    attachModule (m);
}

// create a new circle
bool GuiGraph::newCircle (DlgPoint &point, node_type type)
{
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL) return false;
    
    // get the serial number to use for this node
    int &serial = module->serial ();
    
    // create the new node ...
    DlgCircle *circle = new DlgCircle (point, type, serial, module->node_id ());
    
    // ... add it to the module ...
    module->addNode (circle);
    
    // ... and select it for editing
    module->selectNode (circle);
    
    // see whether creation was cancelled
    if (!editNode ())
    {
        // cleanup
        module->deleteNode ();

        return false;
    }
    
    // update the program state
    GuiDlgedit::window->setMode (NODE_SELECTED);

    // node created -> increase serial number for next node
    serial++;
    
    return true;
}

// create a new arrow
bool GuiGraph::newArrow (DlgPoint &point)
{
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL) return false;

    // calculate absolute position of the point
    point.move (-offset->x (), -offset->y ());

    // get start and end of the circle
    DlgNode *start = module->selected ();
    DlgNode *end = module->getNode (point);

    // sanity checks
    if (!start || start->type () == LINK) return false;
    if (end && end->type () == LINK) return false;
    
    // if no end selected, create a new circle first
    if (end == NULL)
    {
        // chose a sensible type for the new circle
        node_type type = (start->type () == NPC ? PLAYER : NPC);
        
        // there musn't be a node selected to create a new circle
        module->deselectNode ();
        
        // try to create a new circle
        if (newCircle (point, type))
        {
            // chose the newly created circle as end of the arrow
            end = module->getNode (point);
            
            // restore selection
            deselectNode ();
            selectNode (start);
        }
        
        // do we have a valid end now?
        if (end == NULL) return false;
    }

    // no loops and no duplicate connections
    if (start == end || ((DlgCircle *) start)->hasChild (end)) return false; 
    
    // no connection between start and end if both are PLAYER nodes   
    if (start->type () == PLAYER && end->type () == PLAYER) return false;

    // now create the arrow between start and end, ...
    DlgArrow *arrow = new DlgArrow (start, end);

    // ... add it to the module ...
    module->addNode (arrow);
    
    // ... and update everything
    GuiDlgedit::window->list ()->display (start);
	arrow->draw (surface, *offset, graph);
    module->setChanged ();
    
    return true;
}

// add a new subdialogue
bool GuiGraph::newModule (DlgPoint &point)
{
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL) return false;

    // if a project root exists, use that in file selector
    std::string dir = CfgData::data->getBasedir (module->entry ()->project ());

    // otherwise revert to directory last opened
    if (dir == "") dir = GuiDlgedit::window->directory ();
    
    // allow the user to select a module
    GtkWindow *parent = GTK_WINDOW (GuiDlgedit::window->getWindow());
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_OPEN, "Select sub-dialogue to add", dir + "/");
    fs.add_filter ("*"FILE_EXT, "Adonthell Dialogue Source");

    // set shortcuts
    const std::vector<std::string> & projects = CfgData::data->projectsFromDatadir ();
    for (std::vector<std::string>::const_iterator i = projects.begin (); i != projects.end (); i++)
    {
        const std::string &dir = CfgData::data->getBasedir (*i);
        fs.add_shortcut (dir);
    }

    if (fs.run ())
    {
        DlgModule *subdlg = GuiDlgedit::window->loadSubdialogue (fs.getSelection());

        if (subdlg == NULL) return false;

        // set parent of the sub-dialogue
        subdlg->setParent (module);
        
        // set id of the sub-dialogue
        subdlg->setID ();

        // draw the sub-dialogue
        subdlg->initShape (point);
        subdlg->draw (surface, *offset, graph);

        // update the module
        module->setChanged ();
        module->addNode (subdlg);
              
        // update the module tree
        GuiDlgedit::window->tree ()->insert (module, subdlg);
        
        return true;
    }
    
    return false;    
}

// delete the selected node
bool GuiGraph::deleteNode ()
{
    // if there is no module assigned to the view, there is nothing to select
    if (module == NULL) return false;

    // try to delete node
    if (module->deleteNode ())
    {
        // update the program state
        GuiDlgedit::window->setMode (IDLE);
        
        // update the instant preview
        GuiDlgedit::window->list ()->clear ();

        // remove tooltip if it is open        
        if (tooltip) 
        {
            delete tooltip;
            tooltip = NULL;
        }

        // redraw the dialogue
        draw ();
        
        return true;
    }
    
    return false;    
}

// select a node
bool GuiGraph::selectNode (DlgNode *node)
{
    // if there is no module assigned to the view, there is nothing to select
    if (module == NULL) return false;
    
    // a node has been selected
    if (module->selectNode (node))
    {
        // update the program state
        GuiDlgedit::window->setMode (NODE_SELECTED);
        
        // update the instant preview
        GuiDlgedit::window->list ()->display (node);
        
        // redraw the node
        node->draw (surface, *offset, graph);

        return true;
    }
    
    return false;
}

// select the node at the given position
bool GuiGraph::selectNode (DlgPoint &point)
{
    // if there is no module assigned to the view, there is nothing to select
    if (module == NULL) return false;

    // is point within module's boundaries?
    if (!drawing_area.contains (point)) return false;
    
    // calculate absolute position of the point
    point.move (-offset->x (), -offset->y ());
    
    // see if we're over a node
    DlgNode *node = module->getNode (point);
    
    // no node at that position
    if (node == NULL) return false;

    // otherwise select the node
    if (selectNode (node))
    {
        module->traverse ()->select (node);
        return true;
    }
    
    return false;
}

// select parent
bool GuiGraph::selectParent ()
{
    // if there is no module assigned to the view, there is nothing to select
    if (module == NULL) return false;

    // see if a node is currently selected
    DlgNode *selected = module->selected ();

    // if so ...
    if (selected)
    {
        // ... try to retrieve it's parent
        DlgNode *parent = module->traverse ()->up ();

        // deselect current
        deselectNode ();

        // if we have it, then select it
        if (parent) return selectNode (parent);
    }

    // if no node is selected, we simply select the first one
    return selectRoot ();
}

// select the child of a node
bool GuiGraph::selectChild ()
{
    // if there is no module assigned to the view, there is nothing to select
    if (module == NULL) return false;

    // see if a node is currently selected
    DlgNode *selected = module->selected ();

    // if so ...
    if (selected)
    {
        // ... try to retrieve it's child
        DlgNode *child = module->traverse ()->down ();

        // if we have it, then select it
        if (child)
        {
            deselectNode ();
            return selectNode (child);
        }
    }

    // if no node is selected, we simply select the first one
    else return selectRoot ();
    
    return false;
}

// select a sibling of the currently selected node
bool GuiGraph::selectSibling (query_type pos)
{
    // if there is no module assigned to the view, there is nothing to select
    if (module == NULL) return false;

    // see if a node is currently selected
    DlgNode *selected = module->selected ();

    // if so ...
    if (selected)
    {
        DlgNode *sibling;

        // see if selected is arrow
        if (selected->type () == LINK)
            selected = selected->next (FIRST);
        
        // ... try to retrieve it's child
        if (pos == PREV) sibling = module->traverse ()->left ();
        else sibling = module->traverse ()->right ();

        // if we have something now
        if (sibling && sibling != selected)
        {
            deselectNode ();
            return selectNode (sibling);
        }
    }

    // if no node is selected, we simply select the first one
    else return selectRoot ();
    
    return false;
}

// select the first node in the dialogue
bool GuiGraph::selectRoot ()
{
    return selectNode (module->traverse ()->selectRoot (&module->getNodes ()));
}

// deselect a node
void GuiGraph::deselectNode ()
{
    // if there is no module assigned to the view, there is nothing to select
    if (module == NULL) return;
    
    DlgNode *deselected = module->deselectNode ();
    
    if (deselected)
    {
        // update the program state
        GuiDlgedit::window->setMode (IDLE);
        
        // update the instant preview
        GuiDlgedit::window->list ()->clear ();
        
        // redraw the node
        deselected->draw (surface, *offset, graph);
    }
    
    return;
}

// center the view on given node
bool GuiGraph::centerNode (DlgNode *node)
{
    if (module == NULL) return false;

    if (node == NULL && module->selected () != NULL)
        node = module->selected ();
    else return false;

    // calculate the correct offset for the given node
    DlgPoint pos = node->center ().offset (*offset);
    int x, y; 
    
    x = drawing_area.width () / 5;
    y = drawing_area.height () / 5;

    // is node outside the views inner 60% ?
    if (!drawing_area.inflate (-x, -y).contains (pos))
    {
        // then move the view so it is centered on the given point
        DlgPoint o (-(pos.x()-drawing_area.width()/2), -(pos.y()-drawing_area.height()/2));
        offset->move (o);

        draw ();
        return true;
    }
            
    return false;
}

// center whole dialogue in view
void GuiGraph::center ()
{
    if (module == NULL) return;
    
    int min_x, max_x, y;
    
    module->extension (min_x, max_x, y);
    
    GtkAllocation allocation;
    gtk_widget_get_allocation (graph, &allocation);

    int x_off = (allocation.width - (max_x - min_x))/2 - min_x;
    int y_off = -y + 20;
    
    offset->move (x_off, y_off);
}

// edit selected node
bool GuiGraph::editNode ()
{
    if (module == NULL) return false;
    
    // see if a node is currently selected
    DlgNode *selected = module->selected ();

    // disable scrolling (just in case)
    stopScrolling();

    // if we have a sub-dialogue, descent for editing
    if (selected->type () == MODULE)
    {
        switchModule ((DlgModule *) selected);
        return true;
    }

    // if we have a circle, open edit dialog
    if (selected && selected->type () != LINK)
    {
        GtkWindow *parent = GTK_WINDOW (GuiDlgedit::window->getWindow());
        GuiCircle edit (parent, &selected->type (), ((DlgCircle *) selected)->entry (), module->entry ());

	    // editing aborted?
	    if (!edit.run ()) return false;
	
	    // otherwise update everything
        GuiDlgedit::window->list ()->display (selected);
	    selected->draw (surface, *offset, graph);
        module->setChanged ();
        
        return true;
    }
    
    return false;
}

// set everything up for moving nodes around
bool GuiGraph::prepareDragging (DlgPoint &point)
{
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL) return false;

    // calculate absolute position of the point
    point.move (-offset->x (), -offset->y ());
    
    // see if we're over a node
    DlgNode *node = module->getNode (point);

    // Not over a node
    if (node == NULL) return false;

    // if no node selected, select node for dragging
    if (module->selected () == NULL) selectNode (node);

    // else check whether dragged and selected node are the same
    else if (node != module->selected ()) return false;

    // Is dragged node circle or arrow?
    if (node->type () != LINK)
    {
        // circles and modules can be dragged directly
        mover = node;
        
        // remove any tooltip, as it only gets in the way
        if (tooltip)
        {
            delete tooltip;
            tooltip = NULL;
        }
    }
    else
    {
        // arrows have to be attached to a (invisible) mover 
        mover = new DlgMover (point);
        
        // try to attach arrow to mover
        if (!((DlgMover *) mover)->attach ((DlgArrow *) node)) 
        {
            // moving of arrow failed, so clean up
            delete mover;
            mover = NULL;
        }
    }
    
    // if we have a mover, update program state
    if (mover != NULL)
    {
        GuiDlgedit::window->setMode (NODE_DRAGGED);
        module->setState (NODE_DRAGGED);
        
        return true;
    }
    
    return false;    
}

// drag a node around
void GuiGraph::drag (DlgPoint &point)
{
    static int redraw = 0;
    
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL) return;

    // calculate absolute position of the point
    point.move (-offset->x (), -offset->y ());
    
    // move node
    mover->setPos (DlgPoint (point.x () - (point.x () % CIRCLE_DIAMETER), 
                             point.y () - (point.y () % CIRCLE_DIAMETER)));
        
    // update arrows
    for (DlgNode *a = mover->prev (FIRST); a != NULL; a = mover->prev (NEXT))
        ((DlgArrow *) a)->initShape ();

    for (DlgNode *a = mover->next (FIRST); a != NULL; a = mover->next (NEXT))
        ((DlgArrow *) a)->initShape ();
    
    // update view
    switch (redraw) 
    {
        case 0:
        {
            draw ();
            redraw++;
            break;
        }
        case 7:
        {
            redraw = 0;
            break;
        }
        default:
        {
            redraw++;
            break;
        }
    }
}

// stop dragging node
void GuiGraph::stopDragging (DlgPoint &point)
{
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL || mover == NULL) return;

    // calculate absolute position of the point
    point.move (-offset->x (), -offset->y ());
    
    // see whether arrow was dragged
    if (mover->type () == MOVER)
    {
        // see whether we are over a node
        DlgNode *node = module->getNode (point);
        
        // drop the mover onto the node
        ((DlgMover *) mover)->drop (node);
        
        // cleanup
        delete mover;
    }
    
    // if circle moved, realign it to the grid
    else 
    {
        // make sure we drop on an empty location
        DlgNode *node = module->getNode (point);
        while (node != NULL && node != mover && node->type() != LINK)
        {
            point.move (0, 2*CIRCLE_DIAMETER);
            node = module->getNode (point);
        }

        mover->setPos (DlgPoint (point.x () - (point.x () % CIRCLE_DIAMETER), 
                                 point.y () - (point.y () % CIRCLE_DIAMETER)));
        
        // also need to update arrows and reorder children and parents 
        for (DlgNode *a = mover->prev (FIRST); a != NULL; a = mover->prev (NEXT))
        {
            a->prev (FIRST)->removeNext (a);
            a->prev (FIRST)->addNext (a);
            ((DlgArrow *) a)->initShape ();
        }
        
        for (DlgNode *a = mover->next (FIRST); a != NULL; a = mover->next (NEXT))
        {
            a->next (FIRST)->removePrev (a);
            a->next (FIRST)->addPrev (a);
            ((DlgArrow *) a)->initShape ();
        }
    }    
    // update everything
    if (mover->type() == MODULE)
        deselectNode ();
    else
    {    
        GuiDlgedit::window->list ()->display (module->selected ());
        GuiDlgedit::window->setMode (NODE_SELECTED);
        module->setState (NODE_SELECTED);
        module->setChanged ();
    }

    // clear mover
    mover = NULL;

    draw ();
}

// resize the drawing area
void GuiGraph::resizeSurface (GtkWidget *widget)
{
    GtkAllocation allocation;
    gtk_widget_get_allocation (widget, &allocation);

    // delete the old surface
    if (surface) g_object_unref (surface);
    
    // create a new one with the proper size
    surface = gdk_window_create_similar_surface (gtk_widget_get_window(widget), CAIRO_CONTENT_COLOR, allocation.width, allocation.height);

    // init the surface
    if (GuiResources::getColor (GC_GREY)) clear ();
            
    // set the size of the attached dialogues
    drawing_area.resize (allocation.width, allocation.height);
}

// empty the drawing area
void GuiGraph::clear ()
{
    GdkRectangle t;

    GtkAllocation allocation;
    gtk_widget_get_allocation (graph, &allocation);

    cairo_t *cr = cairo_create (surface);
    gdk_cairo_set_source_color(cr, GuiResources::getColor (GC_GREY));
    cairo_rectangle(cr, 0, 0, allocation.width, allocation.height);
    cairo_fill(cr);
    cairo_destroy(cr);

    t.x = 0;
    t.y = 0;
    t.width = allocation.width;
    t.height = allocation.height;

    gdk_window_invalidate_rect (gtk_widget_get_window (graph), &t, FALSE);
}

// draw the graph to the surface
void GuiGraph::draw ()
{
    // nothing to draw
    if (module == NULL) return;
            
    GdkRectangle t;
    std::vector<DlgNode*>::reverse_iterator i;
    std::vector<DlgNode*> nodes = module->getNodes ();
    
    // get visible part of graph
    t.x = -offset->x ();
    t.y = -offset->y ();
    t.width = drawing_area.width ();
    t.height = drawing_area.height ();

    DlgRect rect (t);

    // Clear graph
    cairo_t *cr = cairo_create (surface);
    gdk_cairo_set_source_color(cr, GuiResources::getColor (GC_WHITE));
    cairo_rectangle(cr, 0, 0, t.width, t.height);
    cairo_fill(cr);
    cairo_destroy(cr);

    // normalize rect
    t.x = 0;
    t.y = 0;

    // check for each node, wether it is visible
    for (i = nodes.rbegin (); i != nodes.rend (); i++)
        // draw nodes and arrows
        if ((*i)->contains (rect))
            (*i)->draw (surface, *offset, NULL);

    // draw backing image to screen
    gdk_window_invalidate_rect (gtk_widget_get_window (graph), &t, FALSE);
}

// the mouse has been moved
void GuiGraph::mouseMoved (DlgPoint &point)
{
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL) return;

    // calculate absolute position of the point
    point.move (-offset->x (), -offset->y ());
    
    // see if we're over a node
    DlgNode *node = module->getNode (point);
    
    // get the node that was highlighted before (if any)
    DlgNode *prev = module->highlightNode (node);
    
    // there's no need for action unless old and new highlight differs
    if (prev != node)
    {
        // clear old if necessary
        if (prev != NULL)
        {
            prev->draw (surface, *offset, graph);
            if (tooltip) 
            {
                delete tooltip;
                tooltip = NULL;
            }
        }
        
        // then highlight the new one
        if (node != NULL && gtk_window_is_active(GTK_WINDOW(gtk_widget_get_toplevel(graph))))
        {
            node->draw (surface, *offset, graph, NODE_HILIGHTED);
            tooltip = new GuiTooltip (node);
            tooltip->draw (graph, *offset);
        }
    }
    
    return;
}

// is scrolling allowed?
bool GuiGraph::scrollingAllowed () const
{
    // if there is no module assigned to the view or no nodes
    // in the module yet, there is nothing to do
    if (module == NULL || module->getNodes ().empty ()) return false;
    return true;
}

// the actual scrolling
void GuiGraph::scroll ()
{
    offset->move (scroll_offset.x, scroll_offset.y);
    draw ();
}

// get the toplevel dialogue module
DlgModule *GuiGraph::dialogue ()
{
    // if there is no module assigned to the view, there is nothing to do
    if (module == NULL) return NULL;

    return module->toplevel ();
}
