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
static void on_list_select_child (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    // anything selected at all?
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        // don't show selection
        gtk_tree_selection_unselect_iter (selection, &iter);

        // ignore edit command if in preview mode
        if (GuiDlgedit::window->mode () == L10N_PREVIEW) return;

        DlgNode* node;
        gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, 1, &node, -1);

        GuiGraph *graph = GuiDlgedit::window->graph ();

        // select the node where the error occurred
        graph->deselectNode ();
        graph->selectNode (node);

        // center the view on it
        graph->centerNode ();
        
        // open the edit dialog
        if (graph->editNode ())
        {
            // dialog closed with okay -> assume the error is fixed
            gtk_list_store_remove(GTK_LIST_STORE(model), &iter);

            // close error console if no items remain in the list
            if (gtk_tree_model_iter_n_children (model, NULL) == 0)
                gtk_widget_hide (GTK_WIDGET (user_data));
        }
    }
}

// callback for closing the window
static void on_close_clicked (GtkButton *button, gpointer user_data)
{
    // only hide the window
    gtk_widget_hide (GTK_WIDGET (user_data));
}

// callback when the window is deleted
static void on_delete_window (GtkWidget *window)
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

    // the error console window
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_object_set_data (G_OBJECT (window), "window", window);
    gtk_widget_set_size_request (window, 400, 320);
    gtk_window_set_title (GTK_WINDOW (window), "Error Console");
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    vbox = gtk_vbox_new (FALSE, 0);
    g_object_ref (vbox);
    g_object_set_data_full (G_OBJECT (window), "vbox", vbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);
    gtk_container_set_border_width (GTK_CONTAINER (vbox), 4);

    // vertical scrollbar for the list
    scrolled = gtk_scrolled_window_new (NULL, NULL);
    g_object_ref (scrolled);
    g_object_set_data_full (G_OBJECT (window), "scrolled", scrolled, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (scrolled);
    gtk_box_pack_start (GTK_BOX (vbox), scrolled, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    // the model
    GtkListStore *model = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_STRING, G_TYPE_STRING);

    // the list
    list = gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
    g_object_ref (list);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(list), GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(list), FALSE);
    g_object_set_data_full (G_OBJECT (window), "list", list, (GDestroyNotify)  g_object_unref);
    gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (scrolled), list);
    gtk_widget_show (list);
    
    // create the columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(list), 0, "Errors", renderer, "text", 0, "font", 2, "foreground", 3, NULL);

    // tooltip for the list
    gtk_widget_set_tooltip_text (window, "Click on an error to bring up the corresponding node");

    hseparator1 = gtk_hseparator_new ();
    g_object_ref (hseparator1);
    g_object_set_data_full (G_OBJECT (window), "hseparator1", hseparator1, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (hseparator1);
    gtk_box_pack_start (GTK_BOX (vbox), hseparator1, FALSE, TRUE, 2);

    buttonbox = gtk_hbutton_box_new ();
    g_object_ref (buttonbox);
    g_object_set_data_full (G_OBJECT (window), "buttonbox", buttonbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (buttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), buttonbox, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (buttonbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing (GTK_BOX (buttonbox), 0);

    // close button
    close = gtk_button_new_with_label ("Close");
    g_object_ref (close);
    g_object_set_data_full (G_OBJECT (window), "close", close, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (close);
    gtk_container_add (GTK_CONTAINER (buttonbox), close);
    gtk_container_set_border_width (GTK_CONTAINER (close), 2);
    gtk_widget_set_can_default (close, TRUE);
    gtk_widget_set_tooltip_text (close, "Hide the Error Console");

    // selection listener
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(list));

    g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK(on_list_select_child), window);
    g_signal_connect (G_OBJECT (close), "clicked", G_CALLBACK (on_close_clicked), window);
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (on_delete_window), window);

    // give the focus to the close button
    gtk_widget_grab_focus (close);
    gtk_widget_grab_default (close);
    
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

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
    gtk_list_store_clear (GTK_LIST_STORE(model));

    gtk_widget_show (list);
}

// add an error message to the list
void GuiError::add (std::string error, DlgNode *node)
{
    GtkTreeIter iter;
    GdkColor color = { 0, 27500, 0, 0 };
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(list));
    gchar *font_color = gdk_color_to_string(&color);

    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, error.c_str(), 1, node, 2, "Fixed,Monospace 10", 3, font_color, -1);

    g_free(font_color);
}

// display all error messages
void GuiError::display ()
{
    // show the console
    gtk_widget_show (window);
}
