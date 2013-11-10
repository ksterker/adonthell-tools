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
 * @file gui_list_events.cc
 *
 * @author Kai Sterker
 * @brief Event-callbacks for the instant dialogue preview
 */
 
#include "gui_list_events.h"
#include "gui_dlgedit.h"

// Node selected in preview
void on_list_select (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    // anything selected at all?
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        DlgCircle* circle;
        gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, 1, &circle, -1);

        GuiGraph *graph = GuiDlgedit::window->graph ();

        // change selection in the graph view
        graph->deselectNode ();
        graph->selectNode (circle);
        graph->centerNode ();
    }
}
