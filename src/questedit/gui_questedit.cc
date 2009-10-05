/*
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
 Part of the Adonthell Project http://adonthell.linuxgames.com
 
 Adonthell is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.
 
 Adonthell is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Adonthell; if not, write to the Free Software 
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/**  
 * @file questedit/gui_questedit.cc
 *
 * @author Kai Sterker
 * @brief The quest editor main window.
 */

#include <gtk/gtk.h>

#ifdef MAC_INTEGRATION
#include <ige-mac-integration.h>
#endif

#include "gui_questedit.h"

// Ui definition
static char questedit_ui[] =
#include "questedit.glade.h"
;

// Main Window: on_widget_destroy App
static void on_widget_destroy (GtkWidget * widget, gpointer data)
{
    gtk_main_quit ();
    gtk_widget_destroy (widget);
}

// ctor
GuiQuestedit::GuiQuestedit ()
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();    
	if (!gtk_builder_add_from_string(Ui, questedit_ui, -1, &err)) 
    {
        g_message ("building questedit main window failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get reference to dialog window
    Window = GTK_WIDGET (gtk_builder_get_object (Ui, "main_window"));
    g_signal_connect (Window, "delete-event", G_CALLBACK (on_widget_destroy), (gpointer) NULL); 
    gtk_widget_show_all (Window);

#ifdef MAC_INTEGRATION
    GtkWidget* menu = GTK_WIDGET (gtk_builder_get_object (Ui, "menu_bar"));
    GtkWidget* separator = GTK_WIDGET (gtk_builder_get_object (Ui, "item_quit_separator"));
    GtkWidget* quit_item = GTK_WIDGET (gtk_builder_get_object (Ui, "item_quit"));

    // Mac OSX-Style menu
    gtk_widget_hide (separator);
    gtk_widget_hide (menu);
    
    ige_mac_menu_set_menu_bar (GTK_MENU_SHELL (menu));
    ige_mac_menu_set_quit_menu_item (GTK_MENU_ITEM (quit_item));
    
    // IgeMacMenuGroup *group = ige_mac_menu_add_app_menu_group ();
    // ige_mac_menu_add_app_menu_item (group, GTK_MENU_ITEM (quit_item), NULL);
#endif
    
    /*
    // connect menu signals
    widget = gtk_builder_get_object (Ui, "item_new");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_new), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_load");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_load), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_save");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_save_activate), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_save_as");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_save_as_activate), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_quit");    
    g_signal_connect (widget, "activate", G_CALLBACK (on_widget_destroy), (gpointer) NULL);
    */
}
