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
 * @file gui_list.h
 *
 * @author Kai Sterker
 * @brief Instant dialogue preview
 */

#ifndef GUI_LIST_H
#define GUI_LIST_H

#include "dlg_circle.h"

/**
 * This widget uses a GtkList to display an instant preview of the dialogue.
 * It shows the parents and childs of a selected node and allows to navigate
 * through the dialogue by clicking on a line of text.
 */
class GuiList
{
public:
    /** 
     * Constructor. Main purpose is to create the drawing area for the
     * graph and to register the GTK+ event callbacks used to catch
     * user input for this widget.
     * @param paned The GtkPaned that will hold the list widget.
     */
    GuiList (GtkWidget* paned);
    /**
     * Standard desctructor.
     */
    ~GuiList ();
    
    /**
     * "Draw" the current contents of the list.
     */
    void draw ();
    /**
     * Empties the list and redraws it.
     */
    void clear ();
    /**
     * Displays the given Dlgnode in the list, including all of it's parents
     * and children.
     * @param the node currently selected.
     */
    void display (DlgNode *node);
    
private:
    /**
     * Add an item to the list.
     * @param mode Defines whether the circle is selected or not
     * @param circle The circle whose text is to be added to the list
     */
    void add (int mode, DlgCircle *circle);

    GtkWidget *list;        // The actual GTK+ list widget
};

#endif // GUI_LIST_H
