/*
   $Id: gui_mapedit.cc,v 1.3 2009/05/21 14:28:18 ksterker Exp $

   Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Mapedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Mapedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Mapedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file mapedit/gui_mapedit.cc
 *
 * @author Kai Sterker
 * @brief The Map Editor's main window
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sstream>
#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gdk/gdkkeysyms.h>

#include <base/base.h>

#ifdef MAC_INTEGRATION
#include <ige-mac-integration.h>
#endif

#include "mapedit/map_cmdline.h"
#include "mapedit/map_data.h"
#include "mapedit/gui_mapedit.h"
#include "mapedit/gui_mapedit_events.h"
#include "mapedit/gui_mapview.h"
#include "mapedit/gui_entity_list.h"
#include "mapedit/gui_zone_list.h"

#define MIME_TYPE "application/x-adonthell-map"

/**
 * Icon of the main window
 */
static char * icon_xpm[] = {
"16 16 13 1",
" 	c None",
".	c #888888",
"+	c #777777",
"@	c #999999",
"#	c #111111",
"$	c #222222",
"%	c #000000",
"&	c #333333",
"*	c #555555",
"=	c #666666",
"-	c #444444",
";	c #070707",
">	c #AAAAAA",
"     .+++.@     ",
"    .#$%&**@    ",
"    =%-#=  $*   ",
"    #*+%+  .%-  ",
"   .# .;+   *%= ",
"   =- =;+   .## ",
"   #= =#=    $%.",
"  .%%##%$*==.&%+",
"  +-  =%-@   *%*",
"  #%%$#%$**=.&%+",
"  %-++*#=    $%.",
" =#+  =%+   .%& ",
" -$   =%+   $%. ",
"+%+   +%+  *#+  ",
"-%.   =%-*=$.   ",
"=*.   @=+..>    "};

/**
 * Global pointer to the main window
 */
GuiMapedit *GuiMapedit::window = NULL;

// Create the main window
GuiMapedit::GuiMapedit ()
{
    GtkWidget *vbox;
    GtkWidget *hbox;
    GtkWidget *menu;
    GtkWidget *submenu;
    GtkWidget *menuitem;
    GtkWidget *hpaned, *vpaned;
    
    window = this;
    
    // recent file management
    std::string cmdline = "-g" + MapCmdline::datadir + " -p" + MapCmdline::project + " -m" + MapCmdline::modeldir;
    RecentFiles = new GuiRecentFiles ("mapedit", cmdline);
    RecentFiles->setListener(this);

    // Statusbar for displaying help and error messages
    GtkWidget *status_help = gtk_statusbar_new ();
    gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(status_help), FALSE);
    // message = new GuiMessages (status_help);
        
    // Main Window
    Wnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request (GTK_WIDGET (Wnd), 800, 600);
    g_signal_connect (G_OBJECT (Wnd), "delete_event", G_CALLBACK (on_widget_destroy), NULL);
            
    // Menu Accelerators
    GtkAccelGroup *accel_group = gtk_accel_group_new ();
    
    // Main Windows Menu
    menu = gtk_menu_bar_new ();

    // Attach Menubar
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (Wnd), vbox);
    gtk_widget_show (vbox);

    // File Menu
    submenu = gtk_menu_new ();

    // New
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (1));
    // g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_new_activate), (gpointer) this);
    gtk_widget_show (menuitem);

    // Open
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
    // g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_load_activate), (gpointer) this);
    gtk_widget_show (menuitem);

    // Open Previous >
    menuitem = gtk_menu_item_new_with_label ("Open Previous");
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
    // g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    // menuItem[OPEN_RECENT] = menuitem;
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem), RecentFiles->recentFileMenu());
    
    // Save
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-save", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (3));
    // g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_save_activate), (gpointer) this);
    // menuItem[SAVE] = menuitem;
    
    // Save As
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-save-as", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (4));
    // g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_save_as_activate), (gpointer) this);
    // menuItem[SAVE_AS] = menuitem;
    
    // Seperator
#ifndef MAC_INTEGRATION
    menuitem = gtk_menu_item_new ();
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);
#endif
    
    /*
    // Revert to Saved
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-revert-to-saved", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (7));
    // g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_revert_activate), (gpointer) NULL);
    menuItem[REVERT] = menuitem;
    
    // Close
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-close", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (5));
    // g_signal_connect (G_OBJECT (menuitem), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (menuitem), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_file_close_activate), (gpointer) NULL);
    menuItem[CLOSE] = menuitem;
    
    // Seperator
    menuitem = gtk_menu_item_new ();
    gtk_menu_shell_append (GTK_MENU (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);
     */

    // Quit
    GtkWidget* quit_item = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), quit_item);
    // gtk_object_set_data (GTK_OBJECT (quit_item), "help-id", GINT_TO_POINTER (6));
    // g_signal_connect (G_OBJECT (quit_item), "enter-notify-event", G_CALLBACK (on_display_help), message);
    // g_signal_connect (G_OBJECT (quit_item), "leave-notify-event", G_CALLBACK (on_clear_help), message);
    g_signal_connect (G_OBJECT (quit_item), "activate", G_CALLBACK (on_widget_destroy), (gpointer) NULL);
    
    // Attach File Menu
    menuitem = gtk_menu_item_new_with_mnemonic ("_File");
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);

    // View menu
    submenu = gtk_menu_new ();

    // Zoom in
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-zoom-in", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_KEY_plus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_name(menuitem, "item_zoom_in");
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_model_zoom_in), (gpointer) this);

    // Zoom out
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-zoom-out", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_KEY_minus, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_set_name(menuitem, "item_zoom_out");
    gtk_widget_set_sensitive(menuitem, false);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_model_zoom_out), (gpointer) this);

    // Separator
    menuitem = gtk_menu_item_new ();
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);

    // Zoom 1:1
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-zoom-100", accel_group);
    gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
    gtk_widget_add_accelerator(menuitem, "activate", accel_group, GDK_KEY_1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    g_signal_connect (G_OBJECT (menuitem), "activate", G_CALLBACK (on_model_reset_zoom), (gpointer) this);

    // Attach View Menu
    menuitem = gtk_menu_item_new_with_mnemonic ("_View");
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
    gtk_menu_shell_append (GTK_MENU_SHELL (menu), menuitem);


    gtk_box_pack_start (GTK_BOX (vbox), menu, FALSE, FALSE, 2);
    gtk_widget_show_all (menu);

#ifdef MAC_INTEGRATION
    // Mac OSX-Style menu
    gtk_widget_hide (menu);
    
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
    gtk_window_add_accel_group (GTK_WINDOW (Wnd), accel_group);
    gtk_widget_realize (Wnd);
    
    // Drawing Area
    View = new GuiMapview (hpaned);
    
    // object trees
    GtkWidget *tab = gtk_notebook_new();
    g_signal_connect (G_OBJECT (tab), "switch-page", G_CALLBACK (on_tree_switched), (gpointer) View);
    gtk_paned_add1 (GTK_PANED (hpaned), tab);
    gtk_widget_show (tab);
    
    // Entity list
    EntityList = new GuiEntityList ();
    gtk_notebook_append_page (GTK_NOTEBOOK(tab), EntityList->getWidget(), gtk_label_new ("Entities"));

    // Zone list
    ZoneList = new GuiZoneList ();
    gtk_notebook_append_page (GTK_NOTEBOOK(tab), ZoneList->getWidget(), gtk_label_new ("Zones"));
    
    // Status bars
    hbox = gtk_hbox_new (FALSE, 0);
    g_object_ref (hbox);
    g_object_set_data_full (G_OBJECT (Wnd), "hbox", hbox, (GDestroyNotify) g_object_unref);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
    gtk_widget_set_size_request (hbox, -1, 20);

    // help message
    g_object_ref (status_help);
    g_object_set_data_full (G_OBJECT (Wnd), "status_help", status_help, (GDestroyNotify) g_object_unref);
    gtk_widget_show (status_help);
    gtk_box_pack_start (GTK_BOX (hbox), status_help, TRUE, TRUE, 0);
    gtk_widget_set_size_request (status_help, -1, 20);

    // grid controls
    GtkWidget *gridbox = gtk_hbox_new (FALSE, 4);
    GtkWidget *showGrid = gtk_toggle_button_new_with_label ("Grid Control");
    gtk_box_pack_start (GTK_BOX (gridbox), showGrid, TRUE, TRUE, 0);
    g_signal_connect (G_OBJECT (showGrid), "toggled", G_CALLBACK (on_grid_toggled), (gpointer) View);
    // gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(showGrid), TRUE);
    gtk_widget_set_can_focus (showGrid, false);
    
    GtkWidget *status_grid = gtk_frame_new (NULL);
    gtk_frame_set_shadow_type (GTK_FRAME(status_grid), GTK_SHADOW_IN);
    gtk_container_add (GTK_CONTAINER(status_grid), gridbox);
    gtk_widget_show_all (status_grid);
    gtk_widget_set_size_request (status_grid, 120, 20);

    gtk_box_pack_start (GTK_BOX (hbox), status_grid, FALSE, TRUE, 0);
    
    // coordinates
    StatusCoordinates = gtk_statusbar_new ();
    g_object_ref (StatusCoordinates);
    g_object_set_data_full (G_OBJECT (Wnd), "status_coordinates", StatusCoordinates, (GDestroyNotify) g_object_unref);
    gtk_widget_show (StatusCoordinates);
    gtk_box_pack_start (GTK_BOX (hbox), StatusCoordinates, FALSE, TRUE, 0);
    gtk_widget_set_size_request (StatusCoordinates, 150, -1);
    
    // set the editor's icon    
    GdkPixbuf *icon = gdk_pixbuf_new_from_xpm_data ((const char**) icon_xpm);
    gtk_window_set_icon (GTK_WINDOW(Wnd), icon);
    
    // Display MainWindow
    gtk_widget_show (Wnd);

    // get the current working directory
    Directory = g_get_current_dir ();

    // set initial state
    clear ();
}

// dtor
GuiMapedit::~GuiMapedit ()
{
    delete RecentFiles;
}

void GuiMapedit::newMap ()
{
    MapData *area = new MapData();
    ActiveMap = LoadedMaps.size();
    LoadedMaps.push_back (area);

    initTitle ();

    View->setMap (area);
    EntityList->setMap (area);
    ZoneList->setMap (area);
    
    std::string datadir = base::Paths().user_data_dir ();
    EntityList->setDataDir (std::string (datadir) + "/" + MapCmdline::modeldir);
}

void GuiMapedit::OnRecentFileActivated (const std::string & file)
{
    loadMap (file);
}

// load map from disk
void GuiMapedit::loadMap (const std::string & fname)
{
    MapData *area = new MapData();
    if (!area->load (fname))
    {
        // TODO: display warning
    }
    
    RecentFiles->registerFile(fname, MIME_TYPE);

    ActiveMap = LoadedMaps.size();
    LoadedMaps.push_back (area);
    
    initTitle ();

    View->setMap (area);
    EntityList->setMap (area);
    ZoneList->setMap (area);
    
    gchar *datadir = g_path_get_dirname (fname.c_str());
    EntityList->setDataDir (std::string (datadir) + "/" + MapCmdline::modeldir);
    g_free (datadir);
}

// save map to disk
void GuiMapedit::saveMap (const std::string & fname)
{
    // nothing to save
    if (ActiveMap == -1) return;
    
    MapData *area = LoadedMaps[ActiveMap];
    if (!area->save (fname))
    {
        // error
    }

    initTitle ();
    RecentFiles->registerFile(fname, MIME_TYPE);
}

// sets the window title
void GuiMapedit::initTitle (const bool & modified)
{
    gchar *fname = g_path_get_basename (filename().c_str());
    std::string title = "Adonthell Mapedit v"VERSION" [";

    title += fname;
    title += modified ? "*]" : "]";

    gtk_window_set_title (GTK_WINDOW (Wnd), title.c_str());
    g_free (fname);
}

void GuiMapedit::clear ()
{
    // update the window title
    initTitle ();

    /*
    // make the various menu-items insensitive
    gtk_widget_set_sensitive (menuItem[SAVE], FALSE);
    gtk_widget_set_sensitive (menuItem[SAVE_AS], FALSE);
    gtk_widget_set_sensitive (menuItem[REVERT], FALSE);
    gtk_widget_set_sensitive (menuItem[CLOSE], FALSE);
    */
    
    // empty the display
    View->clear ();
    ActiveMap = -1;
}

// set location in statusbar
void GuiMapedit::setLocation (const int & x, const int & y, const int & z)
{
    std::stringstream location (std::ios::out);
    location << "X " << x << ", Y " << y << ", Z " << z;
    
    gtk_statusbar_pop (GTK_STATUSBAR(StatusCoordinates), 0);
    gtk_statusbar_push (GTK_STATUSBAR(StatusCoordinates), 0, location.str().c_str());
}

// get the full path/name/extension of a dialogue
std::string GuiMapedit::filename () const
{
    std::string fname = "";
    if (ActiveMap < (int) LoadedMaps.size()) 
    {
        fname = LoadedMaps[ActiveMap]->filename();
    }
    
    if (fname.size() == 0)
    {
        fname = Directory + "/untitled.xml";
    }
    
    return fname;
}
