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

#include "map_cmdline.h"
#include "map_data.h"
#include "gui_mapedit.h"
#include "gui_mapedit_events.h"
#include "gui_mapview.h"

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
    
    // Statusbar for displaying help and error messages
    GtkWidget *status_help = gtk_statusbar_new ();
    gtk_statusbar_set_has_resize_grip (GTK_STATUSBAR(status_help), FALSE);
    // message = new GuiMessages (status_help);
        
    // Main Window
    Wnd = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_usize (GTK_WIDGET (Wnd), 800, 600);
    gtk_signal_connect (GTK_OBJECT (Wnd), "delete_event", GTK_SIGNAL_FUNC (on_widget_destroy), NULL);
            
    // Menu Accelerators
    GtkAccelGroup *accel_group = gtk_accel_group_new ();
    
    // Main Windows Menu
    menu = gtk_menu_bar_new ();

    // Attach Menubar
    vbox = gtk_vbox_new (FALSE, 0);
    gtk_container_add (GTK_CONTAINER (Wnd), vbox);
    gtk_widget_show (vbox);

    gtk_box_pack_start (GTK_BOX (vbox), menu, FALSE, FALSE, 2);

    // File Menu
    submenu = gtk_menu_new ();

    // New
    menuitem =gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (1));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_new_activate), (gpointer) this);
    gtk_widget_show (menuitem);

    // Open
    menuitem =  gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_activate), (gpointer) this);
    gtk_widget_show (menuitem);

    // Open Previous >
    menuitem = gtk_menu_item_new_with_label ("Open Previous");
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (2));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    // menuItem[OPEN_RECENT] = menuitem;
    
    // Save
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-save", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (3));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_save_activate), (gpointer) this);
    // menuItem[SAVE] = menuitem;
    
    // Save As
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-save-as", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (4));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_save_as_activate), (gpointer) this);
    // menuItem[SAVE_AS] = menuitem;
    
    // Seperator
    menuitem = gtk_menu_item_new ();
    gtk_menu_append (GTK_MENU (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);
    
    /*
    // Revert to Saved
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-revert-to-saved", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (7));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_revert_activate), (gpointer) NULL);
    menuItem[REVERT] = menuitem;
    
    // Close
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-close", accel_group);
    gtk_container_add (GTK_CONTAINER (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (5));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_close_activate), (gpointer) NULL);
    menuItem[CLOSE] = menuitem;
    
    // Seperator
    menuitem = gtk_menu_item_new ();
    gtk_menu_append (GTK_MENU (submenu), menuitem);
    gtk_widget_set_sensitive (menuitem, FALSE);
     */

    // Quit
    menuitem = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
    gtk_menu_append (GTK_MENU (submenu), menuitem);
    // gtk_object_set_data (GTK_OBJECT (menuitem), "help-id", GINT_TO_POINTER (6));
    // gtk_signal_connect (GTK_OBJECT (menuitem), "enter-notify-event", GTK_SIGNAL_FUNC (on_display_help), message);
    // gtk_signal_connect (GTK_OBJECT (menuitem), "leave-notify-event", GTK_SIGNAL_FUNC (on_clear_help), message);
    gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_widget_destroy), (gpointer) NULL);

    // Attach File Menu
    menuitem = gtk_menu_item_new_with_mnemonic ("_File");
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem), submenu);
    gtk_container_add (GTK_CONTAINER (menu), menuitem);

    gtk_widget_show_all (menu);

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
    
    // Status bars
    hbox = gtk_hbox_new (FALSE, 0);
    g_object_ref (hbox);
    gtk_object_set_data_full (GTK_OBJECT (Wnd), "hbox", hbox, (GtkDestroyNotify) g_object_unref);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 0);
    gtk_widget_set_usize (hbox, -2, 20);

    // help message
    g_object_ref (status_help);
    gtk_object_set_data_full (GTK_OBJECT (Wnd), "status_help", status_help, (GtkDestroyNotify) g_object_unref);
    gtk_widget_show (status_help);
    gtk_box_pack_start (GTK_BOX (hbox), status_help, TRUE, TRUE, 0);
    gtk_widget_set_usize (status_help, -2, 20);

    // coordinates
    StatusCoordinates = gtk_statusbar_new ();
    g_object_ref (StatusCoordinates);
    gtk_object_set_data_full (GTK_OBJECT (Wnd), "status_coordinates", StatusCoordinates, (GtkDestroyNotify) g_object_unref);
    gtk_widget_show (StatusCoordinates);
    gtk_box_pack_start (GTK_BOX (hbox), StatusCoordinates, FALSE, TRUE, 0);
    gtk_widget_set_usize (StatusCoordinates, 150, -2);
    
    // set the editor's icon    
    GdkPixbuf *icon = gdk_pixbuf_new_from_xpm_data ((const char**) icon_xpm);
    gtk_window_set_icon (GTK_WINDOW(Wnd), icon);
    
    // Display MainWindow
    gtk_widget_show (Wnd);

    // init list of previously opened files
    initRecentFiles ();
    
    // get the current working directory
    Directory = g_get_current_dir ();

    // set initial state
    clear ();
}

// dtor
GuiMapedit::~GuiMapedit ()
{
}

// load map from disk
void GuiMapedit::loadMap (const std::string & fname)
{
    MapData *area = new MapData();
    if (area->load (fname))
    {
        ActiveMap = LoadedMaps.size();
        LoadedMaps.push_back (area);
        
        View->setMap (area);
    }
    else
    {
        // error
        delete area;
    }
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
}

// sets the window title
void GuiMapedit::initTitle ()
{
    gchar *title = "Adonthell Map Editor v"_VERSION_;
    /*
    MapModule *module = graph_->dialogue ();
  
    if (module != NULL)
    {
        if (module->changed ())
            title = g_strjoin (NULL, title, " - [", 
                module->shortName ().c_str (), " (modified)]", NULL);
        else
            title = g_strjoin (NULL, title, " - [", 
                module->shortName ().c_str (), "]", NULL);
    }
    */
    gtk_window_set_title (GTK_WINDOW (Wnd), title);
}

// initialize the list of recently opened files
void GuiMapedit::initRecentFiles ()
{
    /*
    GtkWidget *submenu = GTK_MENU_ITEM(menuItem[OPEN_RECENT])->submenu;
    GtkWidget *menuitem;
    
    // first, remove everything from the submenu
    if (submenu != NULL)
        gtk_container_foreach (GTK_CONTAINER (submenu), (GtkCallback) gtk_widget_destroy, NULL);
    else
        submenu = gtk_menu_new ();

    // get list of files
    std::list<std::string> files = CfgData::data->getFiles ();
    
    // now recreate the recent files list
    for (std::list<std::string>::iterator i = files.begin (); i != files.end (); i++)
    {
        menuitem = gtk_menu_item_new_with_label ((*i).c_str ());
        gtk_container_add (GTK_CONTAINER (submenu), menuitem);
        gtk_object_set_user_data (GTK_OBJECT (menuitem), (void *) (*i).c_str ());
        gtk_signal_connect (GTK_OBJECT (menuitem), "activate", GTK_SIGNAL_FUNC (on_file_load_recent_activate), (gpointer) this);
        gtk_widget_show (menuitem);          
    }

    // append submenu
    gtk_menu_item_set_submenu (GTK_MENU_ITEM(menuItem[OPEN_RECENT]), submenu);
    */
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
    if (ActiveMap > 0) return LoadedMaps[ActiveMap]->filename();
    return Directory + "/untitled.xml";
}
