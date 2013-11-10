/*
   Copyright (C) 2002/2003/2004 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file gui_settings.cc
 *
 * @author Kai Sterker
 * @brief The Dialogue settings dialog.
 */

#include <string.h>
#include <sys/stat.h>
#include <iostream>
#include "cfg_data.h"
#include "gui_file.h"
#include "gui_dlgedit.h"
#include "gui_settings.h"

// global pointer to the settings dialog
GuiSettings * GuiSettings::dialog = NULL;

// the project selection has changed
void on_project_changed (GtkComboBox *widget, gpointer user_data)
{
    // set project basedir accordingly
    GuiSettings *settings = (GuiSettings *) user_data;
    settings->setBasedir (settings->getProject());
}

// browse the harddisk for a project
void on_browse_basedir_clicked (GtkButton * button, gpointer user_data)
{
    // if a project root exists, use that in file selector
    std::string dir = CfgData::data->getBasedir (GuiSettings::dialog->getProject ());

    // otherwise revert to directory last opened
    if (dir == "") dir = GuiDlgedit::window->directory ();
    
    GtkWindow *parent = GTK_WINDOW (GuiDlgedit::window->getWindow());
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Select base directory", dir + "/");

    // File selection closed with OK
    if (fs.run ())
    {
        GtkEntry *entry = (GtkEntry *) user_data;
        std::string file = fs.getSelection ();
        unsigned int len = file.length () - 1;

        // check if we have a directory
        struct stat statbuf;
        stat (file.c_str (), &statbuf);

        if (S_ISDIR (statbuf.st_mode))
        {
            // have a directory
            gtk_entry_set_text (entry, file.substr (0, len).c_str ());
        }
        else
        {
            // extract directory from file name
            unsigned int pos = file.rfind ('/');
            gtk_entry_set_text (entry, file.substr (0, pos).c_str ());
        }

        gtk_editable_set_position (GTK_EDITABLE(entry), -1);
    }
}

// OK button pressed
void on_ok_button_clicked (GtkButton * button, gpointer user_data)
{
    GuiSettings::dialog->applyChanges ();
    delete GuiSettings::dialog;
}

// Apply button pressed
void on_apply_button_clicked (GtkButton * button, gpointer user_data)
{
    GuiSettings::dialog->applyChanges ();
}

// callback for closing the window
void on_close_settings (GtkButton * button, gpointer user_data)
{
    delete GuiSettings::dialog;
}

// ctor
GuiSettings::GuiSettings ()
{
    GtkWidget *vbox;
    GtkWidget *frame;
    GtkWidget *table;
    GtkWidget *label;
    GtkWidget *scrolledwindow;
    GtkWidget *hbox;
    GtkWidget *button;
    GtkWidget *hseparator;
    GtkWidget *hbuttonbox;
    GtkWidget *view;

    // the dialog
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    g_object_set_data (G_OBJECT (window), "window", window);
    gtk_widget_set_size_request (window, 400, 320);
    gtk_window_set_title (GTK_WINDOW (window), "Settings");
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    vbox = gtk_vbox_new (FALSE, 2);
    g_object_ref (vbox);
    g_object_set_data_full (G_OBJECT (window), "vbox", vbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    // dialogue settings
    frame = gtk_frame_new ("Dialogue Settings");
    g_object_ref (frame);
    g_object_set_data_full (G_OBJECT (window), "frame", frame, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 4);

    table = gtk_table_new (2, 2, FALSE);
    g_object_ref (table);
    g_object_set_data_full (G_OBJECT (window), "table", table, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (table);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_container_set_border_width (GTK_CONTAINER (table), 4);
    gtk_table_set_col_spacings (GTK_TABLE (table), 8);
    gtk_table_set_row_spacings (GTK_TABLE (table), 4);
    
    // project
    label = gtk_label_new ("Project");
    g_object_ref (label);
    g_object_set_data_full (G_OBJECT (window), "label", label, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 8);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

    // description
    label = gtk_label_new ("Description");
    g_object_ref (label);
    g_object_set_data_full (G_OBJECT (window), "label", label, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(label, 70, -1);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    g_object_ref (scrolledwindow);
    g_object_set_data_full (G_OBJECT (window), "scrolledwindow", scrolledwindow, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (scrolledwindow);
    gtk_table_attach (GTK_TABLE (table), scrolledwindow, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    description = gtk_text_buffer_new (NULL);
    view = gtk_text_view_new_with_buffer (description);
    g_object_ref (view);
    g_object_set_data_full (G_OBJECT (window), "view", view, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (view);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), view);
    gtk_widget_set_tooltip_text (view, "Here goes a view of the dialogue");
    gtk_text_view_set_editable (GTK_TEXT_VIEW (view), TRUE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);

    // project selection
    project = gtk_combo_box_new();
    g_object_ref (project);
    g_object_set_data_full (G_OBJECT (window), "project", project, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (project);
    gtk_table_attach (GTK_TABLE (table), project, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_tooltip_text (project, "The project this dialogue belongs to");
    g_signal_connect (G_OBJECT (project), "changed", G_CALLBACK (on_project_changed), this);
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (project), renderer, TRUE);
    gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (project), renderer, "text", 0, NULL);

    // add available projects to list
    populateProjects ();

    frame = gtk_frame_new ("Project Settings");
    g_object_ref (frame);
    g_object_set_data_full (G_OBJECT (window), "frame", frame, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 4);

    hbox = gtk_hbox_new (FALSE, 8);
    g_object_ref (hbox);
    g_object_set_data_full (G_OBJECT (window), "hbox", hbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (frame), hbox);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 4);

    // base dir entry
    label = gtk_label_new ("Base dir");
    g_object_ref (label);
    g_object_set_data_full (G_OBJECT (window), "label", label, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    gtk_widget_set_size_request (label, 70, -1);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

    basedir = gtk_entry_new ();
    g_object_ref (basedir);
    g_object_set_data_full (G_OBJECT (window), "basedir", basedir, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (basedir);
    gtk_box_pack_start (GTK_BOX (hbox), basedir, TRUE, TRUE, 0);
    gtk_widget_set_tooltip_text (basedir, "The directory root of dialogue sources belonging to above project");

    // browse for base dir
    button = gtk_button_new_with_label ("...");
    g_object_ref (button);
    g_object_set_data_full (G_OBJECT (window), "button", button, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (button);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
    gtk_widget_set_size_request (button, 30, -1);
    gtk_widget_set_tooltip_text (button, "Browse your file system for the base directory");
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_browse_basedir_clicked), basedir);

    hseparator = gtk_hseparator_new ();
    g_object_ref (hseparator);
    g_object_set_data_full (G_OBJECT (window), "hseparator", hseparator, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (hseparator);
    gtk_box_pack_start (GTK_BOX (vbox), hseparator, FALSE, FALSE, 0);

    hbuttonbox = gtk_hbutton_box_new ();
    g_object_ref (hbuttonbox);
    g_object_set_data_full (G_OBJECT (window), "hbuttonbox", hbuttonbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, FALSE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);
    gtk_box_set_spacing (GTK_BOX (hbuttonbox), 0);

    button = gtk_button_new_with_label ("Apply");
    g_object_ref (button);
    g_object_set_data_full (G_OBJECT (window), "button", button, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    gtk_widget_set_can_default (button, TRUE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_apply_button_clicked), this);

    button = gtk_button_new_with_label ("OK");
    g_object_ref (button);
    g_object_set_data_full (G_OBJECT (window), "button", button, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    gtk_widget_set_can_default (button, TRUE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_ok_button_clicked), NULL);

    button = gtk_button_new_with_label ("Cancel");
    g_object_ref (button);
    g_object_set_data_full (G_OBJECT (window), "button", button, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    gtk_widget_set_can_default (button, TRUE);
    g_signal_connect (G_OBJECT (button), "clicked", G_CALLBACK (on_close_settings), NULL);

    // set transient for dialogue editor main window
    gtk_window_set_transient_for (GTK_WINDOW (window), GTK_WINDOW (GuiDlgedit::window->getWindow ()));

    dialog = this;
    entry = NULL;
}

// dtor
GuiSettings::~GuiSettings ()
{
    gtk_widget_destroy (window);
    dialog = NULL;
}

// display the dialog
void GuiSettings::display (DlgModuleEntry * e, const std::string & name)
{
    if (entry != e)
    {
        entry = e;

        // display the contents of the module
        setProject (entry->project ());
        setDescription (entry->description());        
    }

    // set the title
    gchar *title =  g_strjoin (NULL, "Settings - [", name.c_str (), "]", NULL);
    gtk_window_set_title (GTK_WINDOW (window), title);

    // now show the window
    gtk_widget_show (window);
}

// store the user's entries
void GuiSettings::applyChanges ()
{
    // project
    std::string project = getProject ();
    
    // if project changed, update tree
    if (project != entry->project ())
    {
        if (!entry->setProject (project))
           std::cout << "Loading quests/characters failed!\n";
        GuiDlgedit::window->updateProject ();
    }

    // description
    entry->setDescription (getDescription ());

    // basedir
    if (project != "none") 
        CfgData::data->setBasedir (project, getBasedir ());
}

// returns selected option
std::string GuiSettings::getProject ()
{
    GtkTreeIter iter;
    gchar *s = NULL;

    if (gtk_combo_box_get_active_iter (GTK_COMBO_BOX(project), &iter))
    {
        GtkTreeModel *model = gtk_combo_box_get_model (GTK_COMBO_BOX(project));
        gtk_tree_model_get (model, &iter, 0, &s, -1);
    }

    return s ? s : "none";
}

// sets a default option
void GuiSettings::setProject (const std::string & label)
{
    GtkTreeIter iter;
    gchar *option = NULL;
    GtkTreeModel *model = gtk_combo_box_get_model (GTK_COMBO_BOX(project));

    if (gtk_tree_model_get_iter_first(model, &iter))
    {
        do
        {
            gtk_tree_model_get (model, &iter, 0, &option, -1);
            if (strcmp(option, label.c_str()) == 0)
            {
                // found our entry -> set as default and return
                gtk_combo_box_set_active_iter(GTK_COMBO_BOX(project), &iter);

                // update base directory
                setBasedir (label);

                return;
            }
        }
        while (gtk_tree_model_iter_next(model, &iter));
    }

    // that project is not available yet, so add it
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, 0, label.c_str(), -1);
    gtk_combo_box_set_active_iter (GTK_COMBO_BOX(project), &iter);

    // update base directory
    setBasedir (label);
}

// set the module's description
void GuiSettings::setDescription (const std::string & desc)
{
    gtk_text_buffer_set_text (description, desc.c_str (), -1);
}                                 

// get the module's description
std::string GuiSettings::getDescription ()
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds (description, &start, &end);
    
    gchar *tmp = gtk_text_buffer_get_text (description, &start, &end, TRUE);
    std::string text (tmp);
    g_free (tmp);

    return text;
}

// set project base directory
void GuiSettings::setBasedir (const std::string & project)
{
    // do nothing for project "none"
    if (project == "none")
    {
        gtk_entry_set_text (GTK_ENTRY (basedir), "");
        return;       
    }
    
    // get base directory of given project from config file
    std::string dir = CfgData::data->getBasedir (project);

    // update the entry
    gtk_entry_set_text (GTK_ENTRY (basedir), dir.c_str ());
    gtk_editable_set_position (GTK_EDITABLE (basedir), -1);
}

// get project base directory
std::string GuiSettings::getBasedir ()
{
    return gtk_entry_get_text (GTK_ENTRY (basedir));
}

// add available projects to list
void GuiSettings::populateProjects ()
{
    GtkTreeIter iter;
    GtkListStore *model = gtk_list_store_new (1, G_TYPE_STRING);
    gtk_combo_box_set_model(GTK_COMBO_BOX(project), GTK_TREE_MODEL(model));

    std::vector<std::string> projects = CfgData::data->projectsFromDatadir ();
    std::vector<std::string>::iterator i;
    
    for (i = projects.begin (); i != projects.end (); i++)
    {
        gtk_list_store_append(model, &iter);
        gtk_list_store_set(model, &iter, 0, i->c_str(), -1);
    }
}
