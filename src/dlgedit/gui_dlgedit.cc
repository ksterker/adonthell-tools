/*
   Copyright (C) 2002/2003 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file gui_dlgedit.cc
 *
 * @author Kai Sterker
 * @brief The Dialogue Editor's main window
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef MAC_INTEGRATION
#include <ige-mac-integration.h>
#endif

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#define MIME_TYPE "application/x-adonthell-dlg"

#include <algorithm>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <glib/gstdio.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include "gettext.h"
#include "cfg_data.h"
#include "dlg_cmdline.h"
#include "dlg_compiler.h"
#include "gui_code.h"
#include "gui_settings.h"
#include "gui_resources.h"
#include "gui_dlgedit.h"
#include "gui_dlgedit_events.h"

/**
 * Point parser to another file to load sub-dialogue.
 * Defined in loadlg.l
 */
extern void parser_switch_input ();

/**
 * Return to previous input file after loading sub-dialogue - or 
 * after failing to do so.
 * Defined in loadlg.l
 */
extern void parser_restore_input ();

/**
 * Global pointer to the main window
 */
GuiDlgedit *GuiDlgedit::window = NULL;

/**
 * Strings describing the various program states
 */
const char *GuiDlgedit::progState[NUM_MODES] =
    { " IDLE", " SELECTED", " HIGHLIGHTED", " DRAGGED", " PREVIEW" };

// Create the main window
GuiDlgedit::GuiDlgedit ()
{
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *menu;
    GtkWidget *submenu;
    GtkWidget *menuitem;
    GtkWidget *hpaned, *vpaned;
    
    window = this;
    number = 0;
    
    // recent file management
    std::string cmdline = "-g" + DlgCmdline::datadir + " -p" + DlgCmdline::project;
    RecentFiles = new GuiRecentFiles ("dlgedit", cmdline);
    RecentFiles->setListener(this);

    // Statusbar for displaying help and error messages
    GtkWidget *status_help = gtk_statusbar_new ();
    gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(status_help), FALSE);
    message = new GuiMessages (status_help);
        
    // Main Window
    wnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_default_size(GTK_WINDOW(wnd), 800, 600);
    g_signal_connect (G_OBJECT (wnd), "delete_event", G_CALLBACK (on_widget_destroy), NULL);
            
    // Menu Accelerators
    GtkAccelGroup *accel_group = gtk_accel_group_new ();
    
    // Main Windows Menu
    menu = gtk_menu_bar_new ();

    // Attach Menubar
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (wnd), vbox);
    gtk_widget_show (vbox);

    gtk_box_pack_start (GTK_BOX (vbox), menu, FALSE, FALSE, 2);

    // File Menu
    submenu = gtk_menu_new ();

    // New
    menuitem =gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (1));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_new_activate), (gpointer) this);
    gtk_widget_show (menuitem);

    // Open
    menuitem =  gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_load_activate), (gpointer) this);
    gtk_widget_show (menuitem);

    // Open Previous >
    menuitem = gtk_menu_item_new_with_label ("Open Previous");
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), RecentFiles->recentFileMenu());
    
    // Save
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-save", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (3));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_save_activate), (gpointer) this);
    menuItem[SAVE] = menuitem;
     
    // Save As
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-save-as", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (4));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_save_as_activate), (gpointer) this);
    menuItem[SAVE_AS] = menuitem;

    // Seperator
    menuitem = gtk_menu_item_new ();
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);
    
    // Revert to Saved
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-revert-to-saved", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (7));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_revert_activate), (gpointer) NULL);
    menuItem[REVERT] = menuitem;
    
    // Close
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-close", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (5));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_close_activate), (gpointer) NULL);
    menuItem[CLOSE] = menuitem;
    
    // Seperator
#ifndef MAC_INTEGRATION
    menuitem = gtk_menu_item_new ();
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);
#endif

    // Quit
    GtkWidget *quit_item = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), quit_item);
    g_object_set_data (G_OBJECT (quit_item), "help-id", GINT_TO_POINTER (6));
    g_signal_connect (G_OBJECT (quit_item), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (quit_item), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (quit_item), "activate", G_CALLBACK (on_widget_destroy), (gpointer) NULL);

    // Attach File Menu
    menuitem = gtk_menu_item_new_with_mnemonic ("_File");
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
    gtk_container_add (GTK_CONTAINER (menu), menuitem);

    // Dialogue Menu
    submenu = gtk_menu_new ();
    
    // Settings
    menuitem = gtk_image_menu_item_new_with_mnemonic ("_Settings ...");
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), gtk_image_new_from_stock ("gtk-preferences", GTK_ICON_SIZE_MENU));
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_t, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (10));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_dialogue_player_activate), (gpointer) NULL);
    menuItem[SETTINGS] = menuitem;

    // Custom Functions
    menuitem = gtk_image_menu_item_new_with_mnemonic ("_Python Code ...");
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), gtk_image_new_from_stock ("gtk-justify-fill", GTK_ICON_SIZE_MENU));
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_p, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (11));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_dialogue_functions_activate), (gpointer) this);
    menuItem[FUNCTIONS] = menuitem;

    // Seperator
    menuitem = gtk_menu_item_new ();
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);

    // Compile
    menuitem = gtk_image_menu_item_new_with_mnemonic ("_Compile");
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), gtk_image_new_from_stock ("gtk-convert", GTK_ICON_SIZE_MENU));
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_c, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (12));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_dialogue_compile_activate), (gpointer) NULL);
    menuItem[COMPILE] = menuitem;

    // Preview i18n
#ifdef ENABLE_NLS 
    menuitem = gtk_menu_item_new_with_label ("Preview L10n");
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_v, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (13));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_dialogue_preview_activate), (gpointer) this);
    gtk_widget_show (menuitem);
    menuItem[PREVIEW] = menuitem;
#endif
    
    // Run
    menuitem = gtk_image_menu_item_new_with_mnemonic ("_Execute ...");
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menuitem), gtk_image_new_from_stock ("gtk-execute", GTK_ICON_SIZE_MENU));
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    gtk_widget_add_accelerator (menuitem, "activate", accel_group, GDK_e, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (14));
    g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_dialogue_run_activate), (gpointer) NULL);
    gtk_widget_show (menuitem);
    menuItem[RUN] = menuitem;

    // Attach Dialogue Menu
    menuitem = gtk_menu_item_new_with_mnemonic ("_Dialogue");
    gtk_widget_show (menuitem);
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

    // Window Menu
    windowMenu = gtk_menu_new ();

    // Attach Window Menu
    menuitem = gtk_menu_item_new_with_mnemonic ("_Windows");
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), windowMenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

    gtk_widget_show_all (menu);

#ifdef MAC_INTEGRATION
    // Mac OSX-Style menu
    gtk_widget_hide (menu);
    
    mainMenu = menu;
    
    ige_mac_menu_set_menu_bar (GTK_MENU_SHELL (menu));
    ige_mac_menu_set_quit_menu_item (GTK_MENU_ITEM (quit_item));
    
    // IgeMacMenuGroup *group = ige_mac_menu_add_app_menu_group ();
    // ige_mac_menu_add_app_menu_item (group, GTK_MENU_ITEM (quit_item), NULL);
#endif    
    
    vpaned = gtk_vpaned_new ();
    gtk_box_pack_start (GTK_BOX (vbox), vpaned, TRUE, TRUE, 2);
    gtk_widget_show (vpaned);

    hpaned = gtk_hpaned_new ();
    gtk_paned_add1 (GTK_PANED (vpaned), hpaned);
    gtk_widget_show (hpaned);
    
    // Accelerators
    gtk_window_add_accel_group (GTK_WINDOW (wnd), accel_group);
    gtk_widget_realize (wnd);
    
    // Tree
    tree_ = new GuiTree (hpaned);
    
    // Drawing Area
    graph_ = new GuiGraph (hpaned);
    
    // List
    list_ = new GuiList (vpaned);
    
    // Status bars
    hbox = gtk_hbox_new (FALSE, 0);
    g_object_ref (hbox);
    g_object_set_data_full (G_OBJECT (wnd), "hbox", hbox, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
    gtk_widget_set_size_request (hbox, -1, 20);

    // help message
    g_object_ref (status_help);
    g_object_set_data_full (G_OBJECT (wnd), "status_help", status_help, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (status_help);
    gtk_box_pack_start (GTK_BOX (hbox), status_help, TRUE, TRUE, 0);
    gtk_widget_set_size_request (status_help, -1, 20);

    // program mode
    status_mode = gtk_statusbar_new ();
    g_object_ref (status_mode);
    g_object_set_data_full (G_OBJECT (wnd), "status_mode", status_mode, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (status_mode);
    gtk_box_pack_start (GTK_BOX (hbox), status_mode, FALSE, TRUE, 0);
    gtk_widget_set_size_request (status_mode, 150, -1);
    
    // Display MainWindow
    gtk_widget_show (wnd);

    // set the programm mode
    setMode (IDLE);

    // Display welcome message
    message->display (1000);
    
    // get the current working directory
    directory_ = g_get_current_dir ();
    
    // init the resources we will need
    GuiResources::init (graph_->drawingArea ());

    clear ();
}

// dtor
GuiDlgedit::~GuiDlgedit ()
{
    // cleanup if in preview mode
    if (mode_ == L10N_PREVIEW)
    {
        unlink ("/tmp/locale/xy/LC_MESSAGES/preview.mo");
        rmdir ("/tmp/locale/xy/LC_MESSAGES");
        rmdir ("/tmp/locale/xy");
        rmdir ("/tmp/locale/");    
    }
}

// starts a new dialogue
void GuiDlgedit::newDialogue ()
{
    // the new dialogue
    DlgModule *module = initDialogue ("untitled");

    // Display the dialogue
    showDialogue (module);
}

// open recent file
void GuiDlgedit::OnRecentFileActivated (const std::string & file)
{
    loadDialogue (file);
}

// load a new dialogue
void GuiDlgedit::loadDialogue (const std::string &f)
{
    // make sure that file has an absolute path
    std::string file = ((f[0] == '/' || f[1] == ':') ? f : directory_ + std::string ("/") + f);
    gchar *fname = g_path_get_basename (file.c_str ());
    
    // test if we have a valid dialogue
    if (!checkDialogue (file))
    {        
        message->display (-2, fname);
        g_free(fname);
        return;
    }

    // remember the current directory for later use
    directory_ = g_dirname (file.c_str ());
    
    // get the name to use for the dialogue
    std::string filename (fname);
    
    // remove file extension
    unsigned long pos = filename.rfind (FILE_EXT);
    if (pos != filename.npos) filename.erase (pos);

    // the new dialogue
    DlgModule *module = initDialogue (filename);

    // try to load from file
    if (!module->load ())
    {
        message->display (-3, filename.c_str ());
        closeDialogue ();
    }
    
    // Display the dialogue
    else 
    {
        // update list of previously opened files
        RecentFiles->registerFile(file, MIME_TYPE);

        message->display (200);     
        showDialogue (module, true);
    }

    g_free(fname);
}

// load a sub-dialogue
DlgModule* GuiDlgedit::loadSubdialogue (const std::string &file)
{
    // test if we have a valid dialogue
    if (!checkDialogue (file)) return NULL;

    // remember the current directory for later use
    directory_ = g_dirname (file.c_str ());

    // get the name to use for the dialogue
    gchar *fname = g_path_get_basename (file.c_str ());
    std::string filename (fname);
    g_free (fname);

    // remove file extension
    unsigned long pos = filename.rfind (FILE_EXT);
    if (pos != filename.npos) filename.erase (pos);

    // the sub-dialogue
    DlgModule *module = new DlgModule (directory_, filename, "", "");

    // parser needs to read from sub-dialogue source file
    parser_switch_input ();

    // try to load from file
    if (!module->load ())
    {
        delete module;
        parser_restore_input ();
        return NULL;
    }

    // return the sub-dialogue
    return module;    
}

// revert Dialogue to state on disk
void GuiDlgedit::revertDialogue ()
{
    DlgModule *module = graph_->dialogue ();
    if (module == NULL) return;

    // check whether dialogue (still) exists on disk
    if (!checkDialogue (module->fullName ()))
    {
        gtk_widget_set_sensitive (menuItem[REVERT], FALSE);
        message->display (-2, module->name ().c_str ());
        return;
    }

    // cleanup
    module->clear ();
    
    // reload
    if (!module->load ())
    {
        message->display (-3, module->name ().c_str ());
        closeDialogue ();
        return;
    }
    
    // redisplay
    graph_->detachModule ();
    tree_->updateModule (module);
    graph_->attachModule (module);
}

// save a dialogue
void GuiDlgedit::saveDialogue (const std::string &file)
{
    DlgModule *module = graph_->dialogue ();
    if (module == NULL) return;
    
    // remember the current directory for later use
    directory_ = g_dirname (file.c_str ());
    
    // get the filename
    gchar *fname = g_path_get_basename (file.c_str ());
    std::string filename (fname);
    g_free (fname);
    
    // remove file extension
    unsigned long pos = filename.rfind (FILE_EXT);
    if (pos != filename.npos) filename.erase (pos);

    // try to save file
    if (!module->save (directory_, filename)) 
        message->display (-4, filename.c_str ());
    else
    {
        message->display (201);
        
        // update list of previously opened files
        RecentFiles->registerFile(file, MIME_TYPE);

        // update 'Revert to Saved' menu item
        gtk_widget_set_sensitive (menuItem[REVERT], TRUE);
             
        // update the dialogue's name in case it has changed
        tree_->setName (module);
        initMenu ();
    }  
}

// close the dialogue being displayed
void GuiDlgedit::closeDialogue ()
{
    DlgModule *module = graph_->dialogue ();
    if (module == NULL) return;
    
    // check whether dialogue has been saved
    // if (module->hasChanged ())
    
    // remove the dialogue from the list of open dialogues
    dialogues_.erase (remove (dialogues_.begin (), dialogues_.end (), module), dialogues_.end ());
    
    // detach module
    graph_->detachModule ();
    
    // if another dialogue is open, display that one
    if (dialogues_.size () > 0) showDialogue (dialogues_.front ());
    // otherwise just clear the GUI
    else clear ();
    
    // rebuild the 'windows' menu
    initMenu ();
    
    // delete the dialogue
    delete module;
}

// display a certain dialogue
void GuiDlgedit::showDialogue (DlgModule *module, bool center)
{
    // remove the current module from the view
    graph_->detachModule ();
    
    // update the tree view
    // NOTE that this method does some magic: it will select and attach
    // the sub-dialogue of 'module' that has been viewed before. In that
    // case, 'module' must not be attached, as it is the toplevel dialogue.
    tree_->addModule (module);
    
    // attach the dialogue to the view
    // In case of a newly created or (re)loaded 'module', none of it's
    // sub-dialogues will have been in the view. Therefore, the call
    // above can't attach anything. Therefore we can attach 'module'.
    if (!graph_->getAttached ()) graph_->attachModule (module, center);
    
    // update the custom code entry if neccessary
    if (GuiCode::dialog != NULL)
        GuiCode::dialog->display (module->entry (), module->shortName ());
    
    // update the settings if neccessary
    if (GuiSettings::dialog != NULL)
        GuiSettings::dialog->display (module->entry (), module->shortName ());

    // update 'Revert to Saved' menu item
    if (!checkDialogue (module->fullName ()))
        gtk_widget_set_sensitive (menuItem[REVERT], FALSE);
    else 
        gtk_widget_set_sensitive (menuItem[REVERT], TRUE);
             
    // update the window title
    initTitle ();
}

// compile a dialogue
void GuiDlgedit::compileDialogue ()
{
    DlgModule *module = graph_->dialogue ();
    if (module == NULL) return;

    // init the compiler
    DlgCompiler compiler (module);
    
    // compile
    compiler.run ();
    
    // enable the 'run' menuitem after successful compilation
    gtk_widget_set_sensitive (menuItem[RUN], TRUE);
    
    // report success
    message->display (212);
}

// edit the genral dialogu settings
void GuiDlgedit::settings ()
{
    DlgModule *module = graph_->dialogue ();
    if (module == NULL) return;

    // if the dialog isn't already open ...
    if (GuiSettings::dialog == NULL) 
        GuiSettings::dialog = new GuiSettings ();
    
    // otherwise just show it
    std::string project = module->entry ()->project ();
    GuiSettings::dialog->display (module->entry (), module->shortName ());
}

// edit custom code of current module
void GuiDlgedit::customCode ()
{
    DlgModule *module = graph_->dialogue ();
    if (module == NULL) return;
    
    // if the dialog isn't already open ...
    if (GuiCode::dialog == NULL) 
        GuiCode::dialog = new GuiCode ();
    
    // otherwise just show it
    GuiCode::dialog->display (module->entry (), module->shortName ());
}

// preview the translated dialogue
void GuiDlgedit::previewTranslation (const std::string &catalogue)
{
    DlgModule *module = graph_->dialogue ();
    if (module == NULL) return;

    // check if we have a proper catalogue
    if (strncmp (catalogue.substr (catalogue.length ()-3).c_str (), ".mo", 3))
    {
        gchar *cname = g_path_get_basename (catalogue.c_str ());
        message->display (-130, cname);
        g_free (cname);
        return;
    }
    
    // see if the file exists at all
    FILE *exists = g_fopen (catalogue.c_str (), "rb");

    if (!exists)
    {
        gchar *cname = g_path_get_basename (catalogue.c_str ());
        message->display (-2, cname);
        g_free (cname);
        return;
    }
    // if it does, check magic number of catalogue file
    else
    {
        unsigned int magic;
        
        // read magic number
        fread (&magic, 4, 1, exists);
        fclose (exists);
    
        if (magic != 0x950412de)
        {
            gchar *cname = g_path_get_basename (catalogue.c_str ());
            message->display (-130, cname);
            g_free (cname);
            return;
        }
    }
    
    // create temporary locale directory
#ifndef WIN32
    if (mkdir ("/tmp/locale/", 0750) ||
        mkdir ("/tmp/locale/xy", 0750) ||
        mkdir ("/tmp/locale/xy/LC_MESSAGES", 0750))
#else
    if (mkdir ("tmp/locale/") ||
        mkdir ("tmp/locale/xy") ||
        mkdir ("tmp/locale/xy/LC_MESSAGES"))
#endif
    {
        message->display (-131);
        return;
    }
            
#ifndef WIN32
    // create a symlink to the given catalogue
    symlink (catalogue.c_str (), "/tmp/locale/xy/LC_MESSAGES/preview.mo");
    
    // set the language to use  
    setenv ("LANGUAGE", "xy", 1);
#else
    // create a symlink to the given catalogue
    // VISTA only: CreateSymbolicLink ()
    
    // set the language to use  
    SetEnvironmentVariable ("LANGUAGE", "xy");
#endif
    
#ifdef ENABLE_NLS
    {
        // tell gettext that the language has changed
        extern int _nl_msg_cat_cntr;
        ++_nl_msg_cat_cntr;
    }
#endif
    
    // open the catalogue
    bindtextdomain ("preview", "/tmp/locale");
    textdomain ("preview");
    
    // deselect selected node, if any
    DlgNode *node = module->deselectNode ();
    
    // update menuitem
    gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN(menuItem[PREVIEW]))), "Exit Preview mode");
    
    // set program mode
    setMode (L10N_PREVIEW);
    message->display (130);
    
    // reselect node with proper translation
    if (node != NULL) module->selectNode (node);
}

// stop preview mode
void GuiDlgedit::exitPreview ()
{
    DlgModule *module = graph_->dialogue ();

    // clear the program mode
    setMode (NUM_MODES);
    
    // restore the program mode
    if (module != NULL) 
    {
        setMode (module->state ());
        
        // deselect selected node, if any
        DlgNode *node = module->deselectNode ();
    
        // reselect node without translation
        if (node != NULL) module->selectNode (node);
    }
    
    // update menuitem
    gtk_label_set_text (GTK_LABEL (gtk_bin_get_child (GTK_BIN(menuItem[PREVIEW]))), "Preview Translation");

    // cleanup
    unlink ("/tmp/locale/xy/LC_MESSAGES/preview.mo");
    rmdir ("/tmp/locale/xy/LC_MESSAGES");
    rmdir ("/tmp/locale/xy");
    rmdir ("/tmp/locale/");

    // clear the statusbar
    message->clear ();
}

void GuiDlgedit::setChanged ()
{
    // update tree
    tree_->setChanged (graph_->dialogue ());
    
    // update title bar
    initTitle ();
}

void GuiDlgedit::updateProject ()
{
    // update tree
    tree_->updateProject (graph_->dialogue ());
}

bool GuiDlgedit::checkDialogue (const std::string &file)
{
    // first, open the file
    FILE *test = g_fopen (file.c_str (), "rb");

    if (!test)
        return false;
    
    // check if it's a regular file
    struct stat statbuf;
    fstat (fileno (test), &statbuf);
    if (!S_ISREG (statbuf.st_mode))
    {
        fclose (test);
        return false;
    }
    
    loadlgin = test;
    return true;
}

DlgModule *GuiDlgedit::initDialogue (std::string name)
{
    // serial number
    std::string serial = g_strdup_printf ("-%i", ++number);

    // the new dialogue
    DlgModule *dlg = new DlgModule (directory_, name, serial, "New Dialogue");

    // set project if dlgedit started with '-j' option
    dlg->entry()->setProject (DlgCmdline::project);
    
    // insert into the list of open dialogues
    dialogues_.push_back (dlg);

    // rebuild the 'Window' menu
    initMenu ();

    // activate all dialogue related menu-items
    gtk_widget_set_sensitive (menuItem[SAVE], TRUE);
    gtk_widget_set_sensitive (menuItem[SAVE_AS], TRUE);
    gtk_widget_set_sensitive (menuItem[CLOSE], TRUE);
    gtk_widget_set_sensitive (menuItem[SETTINGS], TRUE);
    gtk_widget_set_sensitive (menuItem[FUNCTIONS], TRUE);
    gtk_widget_set_sensitive (menuItem[COMPILE], TRUE);
#ifdef ENABLE_NLS
    gtk_widget_set_sensitive (menuItem[PREVIEW], TRUE);
#endif
    
    return dlg;
}

// sets the window title
void GuiDlgedit::initTitle ()
{
    gchar *title = (char*) "Adonthell Dialogue Editor v"_VERSION_;
    DlgModule *module = graph_->dialogue ();
  
    if (module != NULL)
    {
        if (module->changed ())
            title = g_strjoin (NULL, title, " - [", 
                module->shortName ().c_str (), " (modified)]", NULL);
        else
            title = g_strjoin (NULL, title, " - [", 
                module->shortName ().c_str (), "]", NULL);
    }
    gtk_window_set_title (GTK_WINDOW (wnd), title);
}

// init the 'windows' menu
void GuiDlgedit::initMenu ()
{
    // first, remove everything from the menu
    gtk_container_foreach (GTK_CONTAINER (windowMenu), (GtkCallback)gtk_widget_destroy, NULL);

    // Now rebuild the menu if any dialogues remain
    if (dialogues_.size () == 0) return;

    int position = 0;   
    std::vector<DlgModule*>::iterator i;
    // GtkAccelGroup *accel_group = gtk_accel_group_get_default ();
    GtkWidget *menuitem;
    
    for (i = dialogues_.begin (); i != dialogues_.end (); i++, position++)
    {
        menuitem = gtk_menu_item_new_with_label ((*i)->shortName ().c_str ());
        gtk_container_add (GTK_CONTAINER (windowMenu), menuitem);
        g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_window_activate), (gpointer) *i);
        g_object_set_data (G_OBJECT (menuitem), "help-id", GINT_TO_POINTER (20));
        g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
        g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
        gtk_widget_show (menuitem);
    
        // Menu Accelerators
        // if (position < 9)
        //    gtk_widget_add_accelerator (menuitem, "activate", accel_group, 
        //        GDK_1 + position, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    }
    
#ifdef MAC_INTEGRATION
    ige_mac_menu_sync(GTK_MENU_SHELL(mainMenu));
#endif
}

void GuiDlgedit::clear ()
{
    // update the window title
    initTitle ();

    // make the various menu-items insensitive
    gtk_widget_set_sensitive (menuItem[SAVE], FALSE);
    gtk_widget_set_sensitive (menuItem[SAVE_AS], FALSE);
    gtk_widget_set_sensitive (menuItem[REVERT], FALSE);
    gtk_widget_set_sensitive (menuItem[CLOSE], FALSE);
    gtk_widget_set_sensitive (menuItem[SETTINGS], FALSE);
    gtk_widget_set_sensitive (menuItem[FUNCTIONS], FALSE);
    gtk_widget_set_sensitive (menuItem[COMPILE], FALSE);
#ifdef ENABLE_NLS
    gtk_widget_set_sensitive (menuItem[PREVIEW], FALSE);
#endif
    gtk_widget_set_sensitive (menuItem[RUN], FALSE);
    
    // empty the graph and list widget
    graph_->clear ();
    list_->clear ();
}

void GuiDlgedit::setMode (mode_type mode)
{
    const char *text;
    
    // get the string representing the current program state   
    if (mode < IDLE || mode >= NUM_MODES)
    {
        text = " INVALID MODE";
        mode_ = IDLE;
    }
    else
    {
        // ignore everything else as long as we are in preview mode
        if (mode_ == L10N_PREVIEW) return;
        
        text = progState[mode];
        mode_ = mode;
    }
    
    // some context id the statusbar needs for some reason
    int id = gtk_statusbar_get_context_id (GTK_STATUSBAR (status_mode), "Mode");
    
    // remove the previous message
    gtk_statusbar_pop (GTK_STATUSBAR (status_mode), id);
    
    // add the new status
    gtk_statusbar_push (GTK_STATUSBAR (status_mode), id, text);
}

// get the full path/name/extension of a dialogue
std::string GuiDlgedit::filename ()
{
    DlgModule *module = graph_->dialogue ();
    
    return module->fullName ();
}
