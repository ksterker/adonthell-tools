/*
   $Id: gui_settings.cc,v 1.2 2004/08/02 07:39:24 ksterker Exp $ 

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

#include <gtk/gtk.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
#include "cfg_data.h"
#include "gui_file.h"
#include "gui_dlgedit.h"
#include "gui_settings.h"

// global pointer to the settings dialog
GuiSettings * GuiSettings::dialog = NULL;

// the project selection has changed
void on_project_changed (GtkMenuItem *menuitem, gpointer user_data)
{
    // read project from selected item
    std::string project = (char *) gtk_object_get_user_data (GTK_OBJECT (menuitem));

    // set project basedir accordingly
    GuiSettings *settings = (GuiSettings *) user_data;
    settings->setBasedir (project);
}

// browse the harddisk for a project
void on_browse_basedir_clicked (GtkButton * button, gpointer user_data)
{
    // if a project root exists, use that in file selector
    std::string dir = CfgData::data->getBasedir (GuiSettings::dialog->getProject ());

    // otherwise revert to directory last opened
    if (dir == "") dir = GuiDlgedit::window->directory ();
    
    GuiFile fs (GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, "Select base directory", dir + "/");

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

        gtk_entry_set_position (entry, -1);
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
    GtkWidget *project_menu;
    GtkWidget *hbox;
    GtkWidget *button;
    GtkWidget *hseparator;
    GtkWidget *hbuttonbox;
    GtkWidget *view;
    GtkTooltips *tooltips;

    tooltips = gtk_tooltips_new ();

    // the dialog
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window), "window", window);
    gtk_widget_set_usize (window, 400, 320);
    gtk_window_set_title (GTK_WINDOW (window), "Settings");
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_CENTER);
    gtk_window_set_policy (GTK_WINDOW (window), FALSE, FALSE, FALSE);
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);

    vbox = gtk_vbox_new (FALSE, 2);
    gtk_widget_ref (vbox);
    gtk_object_set_data_full (GTK_OBJECT (window), "vbox", vbox, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox);
    gtk_container_add (GTK_CONTAINER (window), vbox);

    // dialogue settings
    frame = gtk_frame_new ("Dialogue Settings");
    gtk_widget_ref (frame);
    gtk_object_set_data_full (GTK_OBJECT (window), "frame", frame, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 4);

    table = gtk_table_new (2, 2, FALSE);
    gtk_widget_ref (table);
    gtk_object_set_data_full (GTK_OBJECT (window), "table", table, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (table);
    gtk_container_add (GTK_CONTAINER (frame), table);
    gtk_container_set_border_width (GTK_CONTAINER (table), 4);
    gtk_table_set_col_spacings (GTK_TABLE (table), 8);
    gtk_table_set_row_spacings (GTK_TABLE (table), 4);
    
    // project
    label = gtk_label_new ("Project");
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 8);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

    // description
    label = gtk_label_new ("Description");
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_table_attach (GTK_TABLE (table), label, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_usize (label, 70, -2);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow);
    gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow);
    gtk_table_attach (GTK_TABLE (table), scrolledwindow, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

    description = gtk_text_buffer_new (NULL);
    view = gtk_text_view_new_with_buffer (description);
    gtk_widget_ref (view);
    gtk_object_set_data_full (GTK_OBJECT (window), "view", view, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (view);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), view);
    gtk_tooltips_set_tip (tooltips, view, "Here goes a view of the dialogue", NULL);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (view), TRUE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (view), GTK_WRAP_WORD);

    // project selection
    project = gtk_option_menu_new ();
    gtk_widget_ref (project);
    gtk_object_set_data_full (GTK_OBJECT (window), "project", project, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (project);
    gtk_table_attach (GTK_TABLE (table), project, 1, 2, 0, 1, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
    gtk_tooltips_set_tip (tooltips, project, "The project this dialogue belongs to", NULL);
    project_menu = gtk_menu_new ();
    
    // add available projects to list
    populateProjects (project_menu);

    frame = gtk_frame_new ("Project Settings");
    gtk_widget_ref (frame);
    gtk_object_set_data_full (GTK_OBJECT (window), "frame", frame, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame);
    gtk_box_pack_start (GTK_BOX (vbox), frame, FALSE, FALSE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (frame), 4);

    hbox = gtk_hbox_new (FALSE, 8);
    gtk_widget_ref (hbox);
    gtk_object_set_data_full (GTK_OBJECT (window), "hbox", hbox, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox);
    gtk_container_add (GTK_CONTAINER (frame), hbox);
    gtk_container_set_border_width (GTK_CONTAINER (hbox), 4);

    // base dir entry
    label = gtk_label_new ("Base dir");
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox), label, FALSE, FALSE, 0);
    gtk_widget_set_usize (label, 70, -2);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment (GTK_MISC (label), 1, 0.5);

    basedir = gtk_entry_new ();
    gtk_widget_ref (basedir);
    gtk_object_set_data_full (GTK_OBJECT (window), "basedir", basedir, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (basedir);
    gtk_box_pack_start (GTK_BOX (hbox), basedir, TRUE, TRUE, 0);
    gtk_tooltips_set_tip (tooltips, basedir, "The directory root of dialogue sources belonging to above project", NULL);

    // browse for base dir
    button = gtk_button_new_with_label ("...");
    gtk_widget_ref (button);
    gtk_object_set_data_full (GTK_OBJECT (window), "button", button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button);
    gtk_box_pack_start (GTK_BOX (hbox), button, FALSE, FALSE, 0);
    gtk_widget_set_usize (button, 30, -2);
    gtk_tooltips_set_tip (tooltips, button, "Browse your file system for the base directory", NULL);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (on_browse_basedir_clicked), basedir);

    hseparator = gtk_hseparator_new ();
    gtk_widget_ref (hseparator);
    gtk_object_set_data_full (GTK_OBJECT (window), "hseparator", hseparator, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hseparator);
    gtk_box_pack_start (GTK_BOX (vbox), hseparator, FALSE, FALSE, 0);

    hbuttonbox = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox);
    gtk_object_set_data_full (GTK_OBJECT (window), "hbuttonbox", hbuttonbox, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbuttonbox, FALSE, FALSE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox), GTK_BUTTONBOX_END);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox), 0);

    button = gtk_button_new_with_label ("Apply");
    gtk_widget_ref (button);
    gtk_object_set_data_full (GTK_OBJECT (window), "button", button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (on_apply_button_clicked), this);

    button = gtk_button_new_with_label ("OK");
    gtk_widget_ref (button);
    gtk_object_set_data_full (GTK_OBJECT (window), "button", button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (on_ok_button_clicked), NULL);

    button = gtk_button_new_with_label ("Cancel");
    gtk_widget_ref (button);
    gtk_object_set_data_full (GTK_OBJECT (window), "button", button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox), button);
    GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);
    gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (on_close_settings), NULL);

    gtk_object_set_data (GTK_OBJECT (window), "tooltips", tooltips);

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
    GtkMenu *m = (GtkMenu *) gtk_option_menu_get_menu (GTK_OPTION_MENU (project));
    GtkMenuItem *i = (GtkMenuItem *) gtk_menu_get_active (m);
    char *s = (char *) gtk_object_get_user_data (GTK_OBJECT (i));

    return s ? s : "none";
}

// sets a default option
void GuiSettings::setProject (const std::string & label)
{
    GtkMenu *m = (GtkMenu *) gtk_option_menu_get_menu (GTK_OPTION_MENU (project));
    GList *l = gtk_container_children (GTK_CONTAINER (m));                    
    char* c;
    int j = 0;

    while (l)
    {
        GtkMenuItem *i = (GtkMenuItem *) l->data;
        c = (char *) gtk_object_get_user_data (GTK_OBJECT (i));

        if (c && strcmp (c, label.c_str ()) == 0)
        {
            // found our entry -> set as default and return
            gtk_option_menu_set_history (GTK_OPTION_MENU (project), j);

            // update base directory
            setBasedir (label);
            
            return;
        }

        j++;
        l = g_list_next (l);
    }

    // that project is not available yet, so add it
    GtkWidget *menuitem = gtk_menu_item_new_with_label (label.c_str ());
    gtk_object_set_user_data (GTK_OBJECT (menuitem), (void *) strdup (label.c_str ()));
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_project_changed), (gpointer) this);
    gtk_widget_show (menuitem);

    gtk_menu_insert (GTK_MENU (m), menuitem, 0);
    gtk_option_menu_set_history (GTK_OPTION_MENU (project), 0);

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
    gtk_entry_set_position (GTK_ENTRY (basedir), -1);                                             
}

// get project base directory
std::string GuiSettings::getBasedir ()
{
    return gtk_entry_get_text (GTK_ENTRY (basedir));
}

// add available projects to list
void GuiSettings::populateProjects (GtkWidget *menu)
{
    GtkWidget *menuitem;
    std::vector<std::string> projects = CfgData::data->projectsFromDatadir ();
    std::vector<std::string>::iterator i;
    
    for (i = projects.begin (); i != projects.end (); i++)
    {
        menuitem = gtk_menu_item_new_with_label ((*i).c_str ());
        gtk_object_set_user_data (GTK_OBJECT (menuitem), (void *) strdup ((*i).c_str ()));
        gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_project_changed), (gpointer) this);
        gtk_widget_show (menuitem);

        gtk_menu_append (GTK_MENU (menu), menuitem);
    }

    // finally append menu to drop-down list
    gtk_option_menu_set_menu (GTK_OPTION_MENU (project), menu);
}
