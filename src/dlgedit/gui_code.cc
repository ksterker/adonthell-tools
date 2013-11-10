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

#include "gui_code.h"
#include "gui_dlgedit.h"

/**
 * @file gui_code.cc
 *
 * @author Kai Sterker
 * @brief The Custom Code dialog
 */

// OK button pressed
void on_button_ok_clicked (GtkButton *button, gpointer user_data)
{
    GuiCode::dialog->applyChanges ();
    delete GuiCode::dialog;
}

// callback for closing the window
void on_close_window (GtkButton *button, gpointer user_data)
{
    delete GuiCode::dialog;
}

// global pointer to the dialog
GuiCode *GuiCode::dialog = NULL;

// ctor
GuiCode::GuiCode ()
{
    GtkWidget *vbox;
    GtkWidget *notebook;
    GtkWidget *label;
    GtkWidget *hbuttonbox;
    GtkWidget *button_ok;
    GtkWidget *button_cancel;

    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_object_set_data (G_OBJECT (window), "window", window);
    gtk_widget_set_size_request (window, 400, 320);
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    vbox = gtk_vbox_new (FALSE, 2);
    g_object_ref (vbox);
    g_object_set_data_full (G_OBJECT (window), "vbox", vbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    notebook = gtk_notebook_new ();
    g_object_ref (notebook);
    g_object_set_data_full (G_OBJECT (window), "notebook", notebook, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (notebook);
    gtk_box_pack_start (GTK_BOX (vbox), notebook, TRUE, TRUE, 0);

    // entry for import statements
    imports = new GuiEdit (notebook);
    gtk_widget_set_tooltip_text (imports->widget (),
        "Additional modules can be included here");

    label = gtk_label_new ("Imports");
    g_object_ref (label);
    g_object_set_data_full (G_OBJECT (window), "label", label, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 0), label);

    // entry for constructor code
    ctor = new GuiEdit (notebook);
    gtk_widget_set_tooltip_text (ctor->widget (),
        "Code entered here will be added to the dialogue's __init__ method");

    label = gtk_label_new ("Constructor");
    g_object_ref (label);
    g_object_set_data_full (G_OBJECT (window), "label", label, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 1), label);

    // entry for destructor code
    dtor = new GuiEdit (notebook);
    gtk_widget_set_tooltip_text (dtor->widget (),
        "This entry allows you to add additional code to the __del__ method of the dialogue");

    label = gtk_label_new ("Destructor");
    g_object_ref (label);
    g_object_set_data_full (G_OBJECT (window), "label", label, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 2), label);

    // entry for custom methods
    methods = new GuiEdit (notebook);
    gtk_widget_set_tooltip_text (methods->widget (),
        "Custom methods to be inserted into the dialogue");

    label = gtk_label_new ("Methods");
    g_object_ref (label);
    g_object_set_data_full (G_OBJECT (window), "label", label, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), 3), label);

    // buttons
    hbuttonbox = gtk_hbutton_box_new ();
    g_object_ref (hbuttonbox);
    g_object_set_data_full (G_OBJECT (window), "hbuttonbox", hbuttonbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, FALSE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing (GTK_BOX (hbuttonbox), 10);

    // OK button
    button_ok = gtk_button_new_with_label ("OK");
    g_object_ref (button_ok);
    g_object_set_data_full (G_OBJECT (window), "button_ok", button_ok, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (button_ok);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button_ok);
    gtk_widget_set_can_default (button_ok, TRUE);

    // Cancel button
    button_cancel = gtk_button_new_with_label ("Cancel");
    g_object_ref (button_cancel);
    g_object_set_data_full (G_OBJECT (window), "button_cancel", button_cancel, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (button_cancel);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button_cancel);
    gtk_widget_set_can_default (button_cancel, TRUE);

    g_signal_connect (G_OBJECT (button_ok), "clicked", G_CALLBACK (on_button_ok_clicked), NULL);
    g_signal_connect (G_OBJECT (button_cancel), "clicked", G_CALLBACK (on_close_window), NULL);
    g_signal_connect (G_OBJECT (window), "delete_event", G_CALLBACK (on_close_window), window);

    // set transient for dialogue editor main window
    gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (GuiDlgedit::window->getWindow ()));    
    
    dialog = this;
    entry = NULL;
}

// dtor
GuiCode::~GuiCode ()
{
    gtk_widget_destroy (window);
    dialog = NULL;
}

// display the dialog
void GuiCode::display (DlgModuleEntry *e, const std::string &name)
{
    if (entry != e)
    {
        entry = e;
    
        // display the contents of the module
        imports->setText (entry->imports ());
        ctor->setText (entry->ctor ());
        dtor->setText (entry->dtor ());
        methods->setText (entry->methods ());
    }
    
    // set the title
    gchar *title = g_strjoin (NULL, "Custom Python Code - [", 
        name.c_str (), "]", NULL);
    gtk_window_set_title (GTK_WINDOW (window), title);

    // now show the window
    gtk_widget_show (window);
}

// store the user's entries
void GuiCode::applyChanges ()
{
    entry->setImports (imports->getText ());
    entry->setCtor (ctor->getText ());
    entry->setDtor (dtor->getText ());
    entry->setMethods (methods->getText ());
}
