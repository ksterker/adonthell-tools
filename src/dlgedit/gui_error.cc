/*
   $Id: gui_error.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $ 

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
 * @file gui_error.cc
 *
 * @author Kai Sterker
 * @brief The error console
 */

#include <gtk/gtk.h>
#include <pango/pango-font.h>
#include "gui_dlgedit.h"
#include "gui_error.h"

// callback for selecting an item from the list
void on_list_select_child (GtkList *list, GtkWidget *widget, gpointer user_data)
{
    // don't show selection
    gtk_list_unselect_child (list, widget);
    
    // ignore edit command if in preview mode
    if (GuiDlgedit::window->mode () == L10N_PREVIEW) return;

    DlgNode* node = (DlgNode *) gtk_object_get_user_data (GTK_OBJECT (widget));
    GuiGraph *graph = GuiDlgedit::window->graph ();
    
    // select the node where the error occured
    graph->deselectNode ();
    graph->selectNode (node);
    
    // center the view on it
    graph->centerNode ();
    
    // open the edit dialog
    if (graph->editNode ())
    {
        // dialog closed with okay -> assume the error is fixed
        gtk_container_remove (GTK_CONTAINER (list), widget);
        
        // close error console if no items remain in the list
        if (!gtk_container_children (GTK_CONTAINER (list)))
            gtk_widget_hide (GTK_WIDGET (user_data));    
    }
}

// callback for closing the window
void on_close_clicked (GtkButton *button, gpointer user_data)
{
    // only hide the window
    gtk_widget_hide (GTK_WIDGET (user_data));
}

// callback when the window is deleted
void on_delete_window (GtkWidget *window)
{
    gtk_widget_destroy (window);
    
    if (GuiError::console)
        delete GuiError::console;
}

// global pointer to the error console
GuiError *GuiError::console = NULL;

// constructor
GuiError::GuiError ()
{
    GtkWidget *vbox;
    GtkWidget *scrolled;
    GtkWidget *hseparator1;
    GtkWidget *buttonbox;
    GtkWidget *close;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    // the error console window
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window), "window", window);
    gtk_widget_set_usize (window, 400, 320);
    gtk_window_set_title (GTK_WINDOW (window), "Error Console");
    gtk_window_set_policy (GTK_WINDOW (window), FALSE, FALSE, FALSE);
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox);
    gtk_object_set_data_full (GTK_OBJECT (window), "vbox", vbox, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);

    // vertical scrollbar for the list
    scrolled = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolled);
    gtk_object_set_data_full (GTK_OBJECT (window), "scrolled", scrolled, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolled);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    // the list
    list = gtk_list_new ();
    gtk_widget_ref (list);
    gtk_object_set_data_full (GTK_OBJECT (window), "list", list, (GtkDestroyNotify) gtk_widget_unref);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), list);
    gtk_widget_show (list);
    
    // tooltip for the list
    gtk_tooltips_set_tip (tooltips, window, "Click on an error to bring up the corresponding node", 0);

    hseparator1 = gtk_hseparator_new ();
    gtk_widget_ref (hseparator1);
    gtk_object_set_data_full (GTK_OBJECT (window), "hseparator1", hseparator1, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hseparator1);
    gtk_box_pack_start (GTK_BOX (vbox), hseparator1, FALSE, TRUE, 2);

    buttonbox = gtk_hbutton_box_new ();
    gtk_widget_ref (buttonbox);
    gtk_object_set_data_full (GTK_OBJECT (window), "buttonbox", buttonbox, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (buttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), buttonbox, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_END);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (buttonbox), 0);
    gtk_button_box_set_child_ipadding (GTK_BUTTON_BOX (buttonbox), 0, 0);

    // close button
    close = gtk_button_new_with_label ("Close");
    gtk_widget_ref (close);
    gtk_object_set_data_full (GTK_OBJECT (window), "close", close, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (close);
    gtk_container_add (GTK_CONTAINER (buttonbox), close);
    gtk_container_set_border_width (GTK_CONTAINER (close), 2);
    GTK_WIDGET_SET_FLAGS (close, GTK_CAN_DEFAULT);
    gtk_tooltips_set_tip (tooltips, close, "Hide the Error Console", 0);

    gtk_signal_connect (GTK_OBJECT (list), "select_child", GTK_SIGNAL_FUNC (on_list_select_child), window);
    gtk_signal_connect (GTK_OBJECT (close), "clicked", GTK_SIGNAL_FUNC (on_close_clicked), window);
    gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (on_delete_window), window);

    // give the focus to the close button
    gtk_widget_grab_focus (close);
    gtk_widget_grab_default (close);
    
    // set the tooltips
    gtk_object_set_data (GTK_OBJECT (window), "tooltips", tooltips);
    
    // no items in list yet
    items = NULL;
    
    // make sure that there is only one error console
    if (console) delete console;
    console = this;

    // set transient for dialogue editor main window
    gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (GuiDlgedit::window->getWindow ()));    
}

// dtor
GuiError::~GuiError ()
{
    console = NULL;
}

// empty the list
void GuiError::clear ()
{
    gtk_widget_hide (list);
    gtk_list_clear_items (GTK_LIST (list), 0, -1);
    gtk_widget_show (list);
    
    items = NULL;
}

// add an error message to the list
void GuiError::add (std::string error, DlgNode *node)
{
    GtkWidget *label;
    GtkWidget *list_item;
    GdkWindow *list_wnd;
    GdkColor color = { 0, 27500, 0, 0 };

    int w = 380, h;

    // get width to use for label
    list_wnd = gtk_widget_get_parent_window (list);
    if (list_wnd) gdk_window_get_size (list_wnd, &w, &h);

    // create label    
    label = gtk_label_new (error.c_str ());
    gtk_widget_set_usize (label, w - 10, 0);
    gtk_label_set_justify ((GtkLabel *) label, GTK_JUSTIFY_LEFT);
    gtk_label_set_line_wrap ((GtkLabel *) label, TRUE);

    // set colr
    gtk_widget_modify_text (label, GTK_STATE_NORMAL, &color);
    gtk_widget_modify_text (label, GTK_STATE_ACTIVE, &color);
    gtk_widget_modify_text (label, GTK_STATE_PRELIGHT, &color);
    gtk_widget_modify_text (label, GTK_STATE_SELECTED, &color);

    // use a fixed size font
    PangoFontDescription *font_desc = pango_font_description_from_string ("Fixed,Monospace 12");
    gtk_widget_modify_font (label, font_desc);
    pango_font_description_free (font_desc);
    
    gtk_widget_show (label);

    // add label to list-item
    list_item = gtk_list_item_new ();
    gtk_container_add (GTK_CONTAINER (list_item), label);
    gtk_object_set_user_data (GTK_OBJECT (list_item), (gpointer) node);
    gtk_widget_show (list_item);
    
    // add list-item to list
    items = g_list_append (items, list_item);
    
    return;    
}

// display all error messages
void GuiError::display ()
{
    // show the console
    gtk_widget_show (window);
    
    // display the new list
    gtk_widget_hide (list);
    gtk_list_append_items (GTK_LIST (list), items);
    gtk_widget_show (list);
}
