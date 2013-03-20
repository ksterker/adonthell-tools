/*
   $Id: gui_list.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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

#include <gtk/gtk.h>
#include "gui_list.h"
#include "gui_list_events.h"

GuiList::GuiList (GtkWidget *paned)
{
    // Create list widget
    GtkWidget *scrolled = gtk_scrolled_window_new (NULL, NULL);
    gtk_paned_add2 (GTK_PANED (paned), scrolled);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_widget_show (scrolled);
    
    list = gtk_list_new ();
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), list);
    gtk_widget_show (list);
    
    // connect callbacks
    gtk_signal_connect (GTK_OBJECT (list), "select_child", (GtkSignalFunc) on_list_select, this);
    
    // GTK_WIDGET_UNSET_FLAGS (list, GTK_CAN_FOCUS);
    
    // no items in the list yet
    items = NULL;
}

// (re)draw the list
void GuiList::draw ()
{
    gtk_widget_queue_draw (list);
} 

void GuiList::clear ()
{
    gtk_widget_hide (list);
    gtk_list_remove_items (GTK_LIST (list), items);
    gtk_widget_show (list);

    items = NULL;
}

// display instant preview for a given node
void GuiList::display (DlgNode *node)
{
    // make sure we have a valid node
    if (node == NULL || node->type() == MODULE) return;
    
    DlgCircle *circle, *c;
   
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
    
    // draw the new list
    gtk_widget_hide (list);
    gtk_list_append_items (GTK_LIST (list), items);
    gtk_widget_show (list);
}

// add an item to the list
void GuiList::add (int mode, DlgCircle *circle)
{
    GtkWidget *label;
    GtkWidget *list_item;
    GdkWindow *list_wnd;
    GdkColor color;
    GtkStyle *style = gtk_style_copy (gtk_widget_get_default_style ());
    std::string label_text = circle->text ();
    int w, h;
    
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

    // Set list-item colors
    style->fg[0] = color;
    style->bg[2] = color;
    
    // get width to use for label
    list_wnd = gtk_widget_get_parent_window (list);
    gdk_window_get_size (list_wnd, &w, &h);

    // create label    
    label = gtk_label_new (label_text.c_str ());
    gtk_widget_set_style (label, style);
    gtk_widget_set_usize (label, w - 10, 0);
    gtk_label_set_justify ((GtkLabel *) label, GTK_JUSTIFY_LEFT);
    gtk_label_set_line_wrap ((GtkLabel *) label, TRUE);
    gtk_widget_show (label);

    // add label to list-item
    list_item = gtk_list_item_new ();
    gtk_container_add (GTK_CONTAINER(list_item), label);
    gtk_object_set_user_data (GTK_OBJECT (list_item), (gpointer) circle);
    gtk_widget_set_can_focus(list_item, FALSE);
    gtk_widget_show (list_item);
    
    // add list-item to list
    items = g_list_append (items, list_item);
    
    return;
}
