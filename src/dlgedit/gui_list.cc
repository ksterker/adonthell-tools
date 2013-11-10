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
 * @file gui_list.cc
 *
 * @author Kai Sterker
 * @brief Instant dialogue preview widget
 */

#include "gui_list.h"
#include "gui_list_events.h"

GuiList::GuiList (GtkWidget *paned)
{
    // Create list widget
    GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
    gtk_paned_add2 (GTK_PANED (paned), scrolled);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_widget_show (scrolled);
    
    // the model
    GtkListStore *model = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_STRING);

    // the list
    list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
    g_object_ref (list);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(list), GTK_TREE_VIEW_GRID_LINES_NONE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), list);
    gtk_widget_show (list);
    
    // create the columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(list), -1, "Errors", renderer, "text", 0, "foreground", 2, NULL);

    // selection listener
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(list));

    // connect callbacks
    g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK(on_list_select), this);
}

// (re)draw the list
void GuiList::draw ()
{
    gtk_widget_queue_draw (list);
} 

void GuiList::clear ()
{
    gtk_widget_hide (list);

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
    gtk_list_store_clear (GTK_LIST_STORE(model));

    gtk_widget_show (list);
}

// display instant preview for a given node
void GuiList::display (DlgNode *node)
{
    // make sure we have a valid node
    if (node == NULL || node->type() == MODULE) return;
    
    DlgCircle *circle, *c;

    // draw the new list
    gtk_widget_hide (list);

    // clear the list
    clear ();
    
    // make sure the node is a circle
    if (node->type () == LINK) circle = (DlgCircle*) node->next (FIRST);
    else circle = (DlgCircle *) node;
    
    // first of all, display all parents
    c = circle->parent (FIRST);
    
    while (c != NULL)
    {
        add (0, c);
        c = circle->parent (NEXT);
    }
    
    // add selected node
    add (1, circle);
    
    // finally, add all children
    c = circle->child (FIRST);
    
    while (c != NULL)
    {
        add (0, c);
        c = circle->child (NEXT);
    }

    gtk_widget_show (list);
}

// add an item to the list
void GuiList::add (int mode, DlgCircle *circle)
{
    GdkColor color;
    std::string label_text = circle->text ();
    
    // see what sort of node we have
    if (mode == 1)
    {
        color.red = 65535;
            
        if (circle->type () == NPC) color.green = 0;
        else if (circle->type () == NARRATOR) color.green = 32700;
        else color.green = 16350;

        color.blue = 0;
    }
    // parent and child nodes' text
    else
    {
        color.red = 0;
        if (circle->type () == NARRATOR) color.green = 27300;
        else color.green = 0;

        if (circle->type () == NPC) color.blue = 0;
        else if (circle->type () == NARRATOR) color.blue = 15600;
        else color.blue = 35000;
    }

    // add list-item to list
    GtkTreeIter iter;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, label_text.c_str(), 1, circle, 2, gdk_color_to_string(&color), -1);
}
