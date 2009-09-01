/*
 $Id: mdl_cmdline.h,v 1.1 2009/03/29 12:27:27 ksterker Exp $
 
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
 * @file modeller/gui_modeller.cc
 *
 * @author Kai Sterker
 * @brief The modeller main window.
 */

#include <gtk/gtk.h>

#ifdef MAC_INTEGRATION
#include <ige-mac-integration.h>
#endif

#include <sys/param.h>
#include <stdlib.h>

#ifdef WIN32
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#undef PATH_MAX
#define PATH_MAX _MAX_PATH
#endif WIN32

#include <base/base.h>
#include <base/diskio.h>
#include <world/placeable_model.h>

#include "gui_modeller.h"
#include "gui_preview.h"
#include "gui_file.h"

// Ui definition
static char modeller_ui[] =
"<?xml version=\"1.0\"?>"
"<interface>"
  "<requires lib=\"gtk+\" version=\"2.16\"/>"
  "<!-- interface-naming-policy project-wide -->"
  "<object class=\"GtkTreeStore\" id=\"sprite_list\">"
    "<columns>"
      "<column type=\"gchararray\"/>"
      "<column type=\"gpointer\"/>"
    "</columns>"
  "</object>"
  "<object class=\"GtkTreeStore\" id=\"shape_list\">"
    "<columns>"
      "<column type=\"gchararray\"/>"
      "<column type=\"gpointer\"/>"
    "</columns>"
  "</object>"
  "<object class=\"GtkWindow\" id=\"main_window\">"
    "<property name=\"title\" translatable=\"yes\">Adonthell Modeller</property>"
    "<property name=\"resizable\">True</property>"
    "<property name=\"default_width\">800</property>"
    "<property name=\"default_height\">600</property>"
    "<child>"
      "<object class=\"GtkVBox\" id=\"vbox1\">"
        "<property name=\"visible\">True</property>"
        "<property name=\"orientation\">vertical</property>"
        "<child>"
          "<object class=\"GtkMenuBar\" id=\"menu_bar\">"
            "<property name=\"visible\">True</property>"
            "<child>"
              "<object class=\"GtkMenuItem\" id=\"menuitem1\">"
                "<property name=\"visible\">True</property>"
                "<property name=\"label\" translatable=\"yes\">_Datei</property>"
                "<property name=\"use_underline\">True</property>"
                "<child type=\"submenu\">"
                  "<object class=\"GtkMenu\" id=\"menu1\">"
                    "<property name=\"visible\">True</property>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"item_new\">"
                        "<property name=\"label\">gtk-new</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                        "<property name=\"accel_group\">menu_accelerators</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"item_load\">"
                        "<property name=\"label\">gtk-open</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                        "<property name=\"accel_group\">menu_accelerators</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"item_save\">"
                        "<property name=\"label\">gtk-save</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                        "<property name=\"accel_group\">menu_accelerators</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"item_save_as\">"
                        "<property name=\"label\">gtk-save-as</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                        "<property name=\"accel_group\">menu_accelerators</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkSeparatorMenuItem\" id=\"item_quit_separator\">"
                        "<property name=\"visible\">True</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"item_quit\">"
                        "<property name=\"label\">gtk-quit</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                        "<property name=\"accel_group\">menu_accelerators</property>"
                      "</object>"
                    "</child>"
                  "</object>"
                "</child>"
              "</object>"
            "</child>"
          "</object>"
          "<packing>"
            "<property name=\"expand\">False</property>"
            "<property name=\"position\">0</property>"
          "</packing>"
        "</child>"
        "<child>"
          "<object class=\"GtkHBox\" id=\"hbox1\">"
            "<property name=\"visible\">True</property>"
            "<child>"
              "<object class=\"GtkDrawingArea\" id=\"model_area\">"
                "<property name=\"visible\">True</property>"
              "</object>"
              "<packing>"
                "<property name=\"position\">0</property>"
              "</packing>"
            "</child>"
            "<child>"
              "<object class=\"GtkVBox\" id=\"vbox2\">"
                "<property name=\"width_request\">300</property>"
                "<property name=\"visible\">True</property>"
                "<property name=\"orientation\">vertical</property>"
                "<child>"
                  "<object class=\"GtkFrame\" id=\"frame1\">"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"label_xalign\">0</property>"
                    "<child>"
                      "<object class=\"GtkAlignment\" id=\"alignment1\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"left_padding\">12</property>"
                        "<child>"
                          "<object class=\"GtkVBox\" id=\"vbox3\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"orientation\">vertical</property>"
                            "<child>"
                              "<object class=\"GtkScrolledWindow\" id=\"scrolledwindow1\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"hscrollbar_policy\">automatic</property>"
                                "<property name=\"shadow_type\">in</property>"
                                "<child>"
                                  "<object class=\"GtkTreeView\" id=\"sprite_view\">"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"model\">sprite_list</property>"
                                    "<property name=\"headers_visible\">False</property>"       
                                    "<property name=\"headers_clickable\">False</property>"         
                                    "<property name=\"fixed_height_mode\">True</property>"
                                   "</object>"
                                "</child>"
                              "</object>"
                              "<packing>"
                                "<property name=\"position\">0</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkHButtonBox\" id=\"hbuttonbox1\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"spacing\">4</property>"
                                "<property name=\"layout_style\">center</property>"
                                "<child>"
                                  "<object class=\"GtkButton\" id=\"add_sprite\">"
                                    "<property name=\"label\">gtk-add</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">True</property>"
                                    "<property name=\"use_stock\">True</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"expand\">False</property>"
                                    "<property name=\"fill\">False</property>"
                                    "<property name=\"position\">0</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkButton\" id=\"remove_sprite\">"
                                    "<property name=\"label\">gtk-remove</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"sensitive\">False</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">True</property>"
                                    "<property name=\"use_stock\">True</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"expand\">False</property>"
                                    "<property name=\"fill\">False</property>"
                                    "<property name=\"position\">1</property>"
                                  "</packing>"
                                "</child>"
                              "</object>"
                              "<packing>"
                                "<property name=\"expand\">False</property>"
                                "<property name=\"padding\">4</property>"
                                "<property name=\"position\">1</property>"
                              "</packing>"
                            "</child>"
                          "</object>"
                        "</child>"
                      "</object>"
                    "</child>"
                    "<child type=\"label\">"
                      "<object class=\"GtkLabel\" id=\"label1\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"label\" translatable=\"yes\">&lt;b&gt;Sprites&lt;/b&gt;</property>"
                        "<property name=\"use_markup\">True</property>"
                      "</object>"
                    "</child>"
                  "</object>"
                  "<packing>"
                    "<property name=\"position\">0</property>"
                  "</packing>"
                "</child>"
                "<child>"
                  "<object class=\"GtkFrame\" id=\"frame2\">"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"label_xalign\">0</property>"
                    "<child>"
                      "<object class=\"GtkAlignment\" id=\"alignment2\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"left_padding\">12</property>"
                        "<child>"
                          "<object class=\"GtkVBox\" id=\"vbox4\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"orientation\">vertical</property>"
                            "<child>"
                              "<object class=\"GtkScrolledWindow\" id=\"scrolledwindow2\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"hscrollbar_policy\">automatic</property>"
                                "<property name=\"shadow_type\">in</property>"
                                "<child>"
                                  "<object class=\"GtkTreeView\" id=\"shape_view\">"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"model\">shape_list</property>"
                                    "<property name=\"headers_visible\">False</property>"       
                                    "<property name=\"headers_clickable\">False</property>"         
                                    "<property name=\"fixed_height_mode\">True</property>"
                                  "</object>"
                                "</child>"
                              "</object>"
                              "<packing>"
                                "<property name=\"position\">0</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkHButtonBox\" id=\"hbuttonbox2\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"spacing\">4</property>"
                                "<property name=\"layout_style\">center</property>"
                                "<child>"
                                  "<object class=\"GtkButton\" id=\"add_shape\">"
                                    "<property name=\"label\">gtk-add</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"sensitive\">False</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">True</property>"
                                    "<property name=\"use_stock\">True</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"expand\">False</property>"
                                    "<property name=\"fill\">False</property>"
                                    "<property name=\"position\">0</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkButton\" id=\"remove_shape\">"
                                    "<property name=\"label\">gtk-remove</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"sensitive\">False</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">True</property>"
                                    "<property name=\"use_stock\">True</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"expand\">False</property>"
                                    "<property name=\"fill\">False</property>"
                                    "<property name=\"position\">1</property>"
                                  "</packing>"
                                "</child>"
                              "</object>"
                              "<packing>"
                                "<property name=\"expand\">False</property>"
                                "<property name=\"padding\">4</property>"
                                "<property name=\"position\">1</property>"
                              "</packing>"
                            "</child>"
                          "</object>"
                        "</child>"
                      "</object>"
                    "</child>"
                    "<child type=\"label\">"
                      "<object class=\"GtkLabel\" id=\"label5\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"label\" translatable=\"yes\">&lt;b&gt;Shapes&lt;/b&gt;</property>"
                        "<property name=\"use_markup\">True</property>"
                      "</object>"
                    "</child>"
                  "</object>"
                  "<packing>"
                    "<property name=\"position\">1</property>"
                  "</packing>"
                "</child>"
                "<child>"
                  "<object class=\"GtkFrame\" id=\"frame3\">"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"label_xalign\">0</property>"
                    "<child>"
                      "<object class=\"GtkAlignment\" id=\"alignment3\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"left_padding\">12</property>"
                        "<child>"
                          "<object class=\"GtkTable\" id=\"table1\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"n_rows\">4</property>"
                            "<property name=\"n_columns\">4</property>"
                            "<property name=\"column_spacing\">4</property>"
                            "<property name=\"homogeneous\">True</property>"
                            "<child>"
                              "<placeholder/>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkLabel\" id=\"label3\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"label\" translatable=\"yes\">X</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">1</property>"
                                "<property name=\"right_attach\">2</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkLabel\" id=\"label4\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"label\" translatable=\"yes\">Y</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">2</property>"
                                "<property name=\"right_attach\">3</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkLabel\" id=\"label6\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"label\" translatable=\"yes\">Z</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">3</property>"
                                "<property name=\"right_attach\">4</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkLabel\" id=\"label7\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"label\" translatable=\"yes\">Offset</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"top_attach\">1</property>"
                                "<property name=\"bottom_attach\">2</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkLabel\" id=\"label8\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"label\" translatable=\"yes\">Position</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"top_attach\">2</property>"
                                "<property name=\"bottom_attach\">3</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkLabel\" id=\"label9\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"label\" translatable=\"yes\">Size</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"top_attach\">3</property>"
                                "<property name=\"bottom_attach\">4</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"offset_x\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">1</property>"
                                "<property name=\"right_attach\">2</property>"
                                "<property name=\"top_attach\">1</property>"
                                "<property name=\"bottom_attach\">2</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"offset_y\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">2</property>"
                                "<property name=\"right_attach\">3</property>"
                                "<property name=\"top_attach\">1</property>"
                                "<property name=\"bottom_attach\">2</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<!-- child>"
                              "<object class=\"GtkEntry\" id=\"offset_z\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">3</property>"
                                "<property name=\"right_attach\">4</property>"
                                "<property name=\"top_attach\">1</property>"
                                "<property name=\"bottom_attach\">2</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child -->"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"pos_x\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">1</property>"
                                "<property name=\"right_attach\">2</property>"
                                "<property name=\"top_attach\">2</property>"
                                "<property name=\"bottom_attach\">3</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"pos_y\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">2</property>"
                                "<property name=\"right_attach\">3</property>"
                                "<property name=\"top_attach\">2</property>"
                                "<property name=\"bottom_attach\">3</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"pos_z\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">3</property>"
                                "<property name=\"right_attach\">4</property>"
                                "<property name=\"top_attach\">2</property>"
                                "<property name=\"bottom_attach\">3</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"size_x\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">1</property>"
                                "<property name=\"right_attach\">2</property>"
                                "<property name=\"top_attach\">3</property>"
                                "<property name=\"bottom_attach\">4</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"size_y\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">2</property>"
                                "<property name=\"right_attach\">3</property>"
                                "<property name=\"top_attach\">3</property>"
                                "<property name=\"bottom_attach\">4</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                            "<child>"
                              "<object class=\"GtkEntry\" id=\"size_z\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"invisible_char\">&#x25CF;</property>"
                                "<property name=\"width_chars\">3</property>"
                              "</object>"
                              "<packing>"
                                "<property name=\"left_attach\">3</property>"
                                "<property name=\"right_attach\">4</property>"
                                "<property name=\"top_attach\">3</property>"
                                "<property name=\"bottom_attach\">4</property>"
                                "<property name=\"x_options\">GTK_FILL</property>"
                              "</packing>"
                            "</child>"
                          "</object>"
                        "</child>"
                      "</object>"
                    "</child>"
                    "<child type=\"label\">"
                      "<object class=\"GtkLabel\" id=\"label2\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"label\" translatable=\"yes\">&lt;b&gt;Edit&lt;/b&gt;</property>"
                        "<property name=\"use_markup\">True</property>"
                      "</object>"
                    "</child>"
                  "</object>"
                  "<packing>"
                    "<property name=\"position\">2</property>"
                  "</packing>"
                "</child>"
              "</object>"
              "<packing>"
                "<property name=\"expand\">False</property>"
                "<property name=\"fill\">False</property>"
                "<property name=\"position\">1</property>"
              "</packing>"
            "</child>"
          "</object>"
          "<packing>"
            "<property name=\"position\">1</property>"
          "</packing>"
        "</child>"
        "<child>"
          "<object class=\"GtkStatusbar\" id=\"statusbar1\">"
            "<property name=\"visible\">True</property>"
            "<property name=\"spacing\">2</property>"
          "</object>"
          "<packing>"
            "<property name=\"expand\">False</property>"
            "<property name=\"position\">2</property>"
          "</packing>"
        "</child>"
      "</object>"
    "</child>"
  "</object>"
  "<object class=\"GtkAccelGroup\" id=\"menu_accelerators\"/>"
"</interface>";

// Main Window: on_widget_destroy App
static void on_widget_destroy (GtkWidget * widget, gpointer data)
{
    gtk_main_quit ();
    gtk_widget_destroy (widget);
}

// File Menu: New
static void on_file_new (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->newModel();
}

// File Menu: Load
static void on_file_load (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    GtkWindow *parent = GTK_WINDOW(modeller->getWindow());

    // get directory
    std::string directory = modeller->modelDirectory();
    if (directory.size() == 0)
    {
        directory = base::Paths.user_data_dir();
    }
    
    // open file chooser
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_OPEN, "Open Model", directory);
    fs.add_filter ("*.xml", "Adonthell Model");
    
    // File selection closed with OK
    if (fs.run ()) modeller->loadModel (fs.getSelection ());
}

// File Menu: Save As
static void on_file_save_as_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    GtkWindow *parent = GTK_WINDOW(modeller->getWindow());

    std::string saveDir;
    std::string filename = modeller->filename ();
    
    /*
    // filename might end in .png if we loaded a 'default' sprite
     
     * Actually, for now the filename will always be 'untitled.xml' if
     * we are working on a new model or end in '.xml' if we loaded an
     * existing model.
    if (filename.find (".png", filename.size() - 4) != std::string::npos)
    {
        filename = filename.replace (filename.size() - 3, 3, "xml");
    }
    */
    
    // generate save directory from sprite directory for new models
    if (filename.find ("untitled") != filename.npos)
    {        
        saveDir = modeller->spriteDirectory ();
        size_t index = saveDir.find ("/gfx/"); 
        if (index != std::string::npos)
        {
            saveDir.replace (index, 4, "/models");
        }
    }
    // otherwise save to directory the model came from
    else
    {
        saveDir = modeller->modelDirectory ();
    }
    
    // try to create directory, if it doesn't exist
    // TODO: make a program setting for that instead of doing it by default 
    // g_mkdir_with_parents (spriteDir.c_str(), 0755);
        
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_SAVE, "Save Model", saveDir + "/" + modeller->filename ());
    fs.add_filter ("*.xml", "Adonthell Model");
    
    // File selection closed with OK
    if (fs.run ()) modeller->saveModel (fs.getSelection ());
}

// File Menu: Save
static void on_file_save_activate (GtkMenuItem * menuitem, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    std::string filename = modeller->filename ();
    std::string dirname = modeller->modelDirectory ();
    
    if (filename.find ("untitled") == filename.npos)
    {
        // only save models that aren't 'untitled'
        std::string dirname = modeller->modelDirectory ();
        modeller->saveModel (dirname + "/" + filename);
    }
    else
    {
        // otherwise open file selection
        on_file_save_as_activate (menuitem, user_data);
    }
}

// callback for selection changes in sprite list
static void anim_selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *tree_model;
    GtkTreeIter iter;
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {
        GuiModeller *modeller = (GuiModeller *) user_data;

        // have we selected a leaf?
        if (!gtk_tree_model_iter_has_child (tree_model, &iter))
        {
            gchar *anim_name;
            gpointer data = NULL;

            // disable del_sprite button
            modeller->setActive ("remove_sprite", false);

            // get name of selected animation
            gtk_tree_model_get (tree_model, &iter, 0, &anim_name, -1);

            // get selected model
            GtkTreeIter parent;
            gtk_tree_model_iter_parent (tree_model, &parent, &iter);
            gtk_tree_model_get (tree_model, &parent, 1, &data, -1);            
            
            if (data != NULL)
            {
                world::placeable_model *model = (world::placeable_model *) data;
                
                // make selected shape current in model ...
                model->set_shape (anim_name);

                // reset preview
                modeller->getPreview ()->setCurShape (NULL);
                
                // and display it in editor
                modeller->updateShapeList (model);
            }
            
            // cleanup
            g_free (anim_name);
        }
        else
        {
            // activate del_sprite button
            modeller->setActive ("remove_sprite", true);
        }
    }
}

// shape selected in the shape list
static void shape_selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *tree_model;
    GtkTreeIter iter, parent;
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {
        GuiModeller *modeller = (GuiModeller *) user_data;
        
        // have we selected a leaf?
        if (gtk_tree_model_iter_parent (tree_model, &parent, &iter))
        {
            // enable remove_shape button
            modeller->setActive ("remove_shape", true);
            
            world::cube3 *cube = NULL;
            gtk_tree_model_get (tree_model, &iter, 1, &cube, -1);   
            if (cube != NULL)
            {
                // and display it in editor
                modeller->getPreview ()->setCurShape (cube);
            }
        }
        else
        {
            // disable remove_shape button
            modeller->setActive ("remove_shape", false);
        }
    }
}

// load sprite from file
static void on_add_sprite_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    GtkWindow *parent = GTK_WINDOW(modeller->getWindow());
    
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_OPEN, "Load Sprite", modeller->spriteDirectory ());
    fs.add_filter ("*.xml|*.png", "Adonthell Sprite");
    
    // File selection closed with OK
    if (fs.run ()) modeller->addSprite (fs.getSelection ());
}

// remove sprite from model
static void on_remove_sprite_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->removeSprite ();
}

// add shape to model
static void on_add_shape_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->addShape ();
}

// remove shape from model
static void on_remove_shape_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->removeShape ();
}

// ctor
GuiModeller::GuiModeller ()
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    Filename = "untitled.xml";
    
	if (!gtk_builder_add_from_string(Ui, modeller_ui, -1, &err)) 
    {
        g_message ("building modeller main window failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get entries linked to the preview
    GtkEntry **shapeData = new GtkEntry*[8];
    shapeData[0] = GTK_ENTRY (gtk_builder_get_object (Ui, "offset_x"));
    shapeData[1] = GTK_ENTRY (gtk_builder_get_object (Ui, "offset_y"));
    shapeData[2] = GTK_ENTRY (gtk_builder_get_object (Ui, "pos_x"));
    shapeData[3] = GTK_ENTRY (gtk_builder_get_object (Ui, "pos_y"));
    shapeData[4] = GTK_ENTRY (gtk_builder_get_object (Ui, "pos_z"));
    shapeData[5] = GTK_ENTRY (gtk_builder_get_object (Ui, "size_x"));
    shapeData[6] = GTK_ENTRY (gtk_builder_get_object (Ui, "size_y"));
    shapeData[7] = GTK_ENTRY (gtk_builder_get_object (Ui, "size_z"));
    
    // setup preview -- needs to happen before window is realized
    widget = gtk_builder_get_object (Ui, "model_area");
    Preview = new GuiPreview (GTK_WIDGET (widget), shapeData);
    
    // get reference to dialog window
    Window = GTK_WIDGET (gtk_builder_get_object (Ui, "main_window"));
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
        
    // connect signals
    widget = gtk_builder_get_object (Ui, "item_new");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_new), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_load");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_load), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_save");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_save_activate), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_save_as");
    g_signal_connect (widget, "activate", G_CALLBACK (on_file_save_as_activate), (gpointer) this);

    widget = gtk_builder_get_object (Ui, "add_sprite");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_sprite_pressed), this);
    widget = gtk_builder_get_object (Ui, "remove_sprite");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_remove_sprite_pressed), this);
    widget = gtk_builder_get_object (Ui, "add_shape");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_shape_pressed), this);
    widget = gtk_builder_get_object (Ui, "remove_shape");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_remove_shape_pressed), this);
    widget = gtk_builder_get_object (Ui, "item_quit");    
    g_signal_connect (widget, "activate", G_CALLBACK (on_widget_destroy), (gpointer) NULL);
    
    // set tree columns and signals
    // FIXME: this could be done in the ui description, but my glade appears buggy in that area.
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    widget = gtk_builder_get_object (Ui, "sprite_view");
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(widget), -1, "Sprites", renderer, "text", 0, NULL);
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
    g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(anim_selected_event), this);
    
    widget = gtk_builder_get_object (Ui, "shape_view");
    gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(widget), -1, "Shapes", renderer, "text", 0, NULL);
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
    g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(shape_selected_event), this);
}

// start creating a new model
void GuiModeller::newModel ()
{
    // cleanup any previous stuff
    gtk_tree_store_clear (GTK_TREE_STORE(gtk_builder_get_object (Ui, "sprite_list")));
    gtk_tree_store_clear (GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list")));
    
    Preview->setCurModel (NULL);
    Preview->setCurShape (NULL);
    
    // reset state
    Filename = "untitled.xml";
    
    // set initial button state
    setActive("add_shape", false);
    setActive("remove_shape", false);
    setActive("remove_sprite", false);
}

// load model from disk
void GuiModeller::loadModel (const std::string & name)
{
    base::diskio placeable (base::diskio::BY_EXTENSION);
    if (!placeable.get_record (name))
    {
        // FIXME: display error in status line instead
        fprintf (stderr, "*** loadModel: error loading model >%s<\n", name.c_str());
        return;
    }
    
    // remember path to model for convenience
    gchar* model_path = g_path_get_dirname (name.c_str());
    ModelDir = model_path;
    g_free (model_path);
    
    // remember model file for convenience
    gchar* model_file = g_path_get_basename (name.c_str());
    Filename = model_file;
    g_free (model_file);
    
    base::flat entity = placeable.get_flat ("entity");
    
    char *id;
    void *value;
    u_int32 size;
    
    // iterate over placeable object
    while (entity.next (&value, &size, &id) != base::flat::T_UNKNOWN)
    {
        // found model data
        if (strcmp ("model", id) == 0)
        {            
            // load model
            world::placeable_model * model = new world::placeable_model ();
            model->get_state (entity);
            
            // and add it to the UI
            addModel (model);
        }
    }
}    

// save model to disk
void GuiModeller::saveModel (const std::string & name)
{
    // remember path to model for convenience
    gchar* model_path = g_path_get_dirname (name.c_str());
    ModelDir = model_path;
    g_free (model_path);

    // remember model file for convenience
    gchar* model_file = g_path_get_basename (name.c_str());
    Filename = model_file;
    g_free (model_file);
    
    base::flat entity;
    world::placeable_model *model = NULL;

    GtkTreeIter iter, child;
    GtkTreeModel *tree_model = GTK_TREE_MODEL(gtk_builder_get_object (Ui, "sprite_list"));
    
    // iterate over models in sprite_tree
    if (gtk_tree_model_get_iter_first (tree_model, &iter))
    {
        // get default state
        if (gtk_tree_model_iter_children (tree_model, &child, &iter))
        {
            gchar *state = NULL;
            gtk_tree_model_get (tree_model, &child, 0, &state, -1);
            if (state != NULL)
            {
                entity.put_string ("state", state);
                g_free (state);
            }
        }
        
        do
        {
            gtk_tree_model_get (tree_model, &iter, 1, &model, -1);
            if (model != NULL)
            {
                model->put_state (entity);
            }
        }
        while (gtk_tree_model_iter_next (tree_model, &iter));
    }
    
    // create list "entity" with string "state"
    base::diskio placeable (base::diskio::BY_EXTENSION);
    placeable.put_flat ("entity", entity);
    placeable.put_record (name);
}

// add a new sprite to the model
void GuiModeller::addSprite (const std::string & name)
{
    // remember path to sprite for convenience
    gchar* sprite_dir = g_path_get_dirname (name.c_str());
    SpriteDir = sprite_dir;
    g_free (sprite_dir);

    // create new model
    world::placeable_model *model = new world::placeable_model();
    std::string base_path = base::Paths.user_data_dir();
    std::string sprite_path = name;
    
    // make sure to use path relative to (user defined) data directory
    if (base_path == "" || !getRelativeSpritePath (sprite_path, base_path))
    {
        // fallback to builtin data dir if that doesn't seem to work
        base_path = base::Paths.game_data_dir();
        if (!getRelativeSpritePath (sprite_path, base_path))
        {
            // if everythin fails, try locating gfx/ in the path and use 
            // that as relative path
            size_t pos = sprite_path.rfind ("gfx/");
            if (pos != std::string::npos)
            {
                sprite_path = sprite_path.substr (pos);
            }
        }
    }

    // still couldn't create a relative sprite path
    if (g_path_is_absolute (sprite_path.c_str()))
    {
        // FIXME: display error in status bar
        printf ("*** warning: cannot create sprite path relative to data directory!\n");
    }
    
    // set relatice sprite name
    model->set_sprite (sprite_path);
    
    // and add it to the UI
    addModel (model);
}

// add existing sprite to the model
void GuiModeller::addModel (world::placeable_model *model)
{
    // try loading sprite
    gfx::sprite *sprt = model->get_sprite ();
    
    if (sprt != NULL)
    {
        gchar *sprite_name = g_path_get_basename (sprt->filename().c_str());
        
        GtkTreeIter sprite_iter;
        GtkTreeIter anim_iter;
        
        GtkTreeStore *sprite_store = GTK_TREE_STORE(gtk_builder_get_object (Ui, "sprite_list"));
        GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "sprite_view"));
        
        // add to sprite list
        gtk_tree_store_append (sprite_store, &sprite_iter, NULL);
        gtk_tree_store_set (sprite_store, &sprite_iter, 0, sprite_name, 1, (gpointer) model, -1);

        // default anim for selection
        // FIXME: newly created model has no current shape
        // std::string cur_anim = model->current_shape_name();

        // add animations of the sprite to the sprite list
        for (gfx::sprite::animation_map::const_iterator anim = sprt->begin(); anim != sprt->end(); anim++)
        {
            // add corresponding shape to model
            model->add_shape (anim->first);
            
            gtk_tree_store_append (sprite_store, &anim_iter, &sprite_iter);
            gtk_tree_store_set (sprite_store, &anim_iter, 0, anim->first.c_str(), -1);
            
            // select default animation
            if (anim == sprt->begin())
            {
                GtkTreePath *path = gtk_tree_model_get_path (GTK_TREE_MODEL(sprite_store), &anim_iter);
                gtk_tree_view_expand_to_path (tree_view, path);
                
                GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
                gtk_tree_selection_select_path (selection, path);
                
                gtk_tree_path_free (path);
            }            
        }
        
        // cleanup
        g_free (sprite_name);
    }
}

// remove sprite from model
void GuiModeller::removeSprite ()
{
    GtkTreeIter iter, child;
    GtkTreeModel *tree_model;
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "sprite_view"));
    GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
    
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {        
        // get selected model
        world::placeable_model *model = NULL;
        gtk_tree_model_get (tree_model, &iter, 1, &model, -1);
        if (model != NULL)
        {
            // model is not part of anything
            delete model;

            // remove model from interface
            if (gtk_tree_store_remove (GTK_TREE_STORE(tree_model), &iter))
            {
                // select new sprite
                if (gtk_tree_model_iter_children (tree_model, &child, &iter))
                {
                    GtkTreePath *path = gtk_tree_model_get_path (tree_model, &child);
                    gtk_tree_selection_select_path (selection, path);                
                    gtk_tree_path_free (path);
                    return;
                }
            }
            
            // clear shape list
            gtk_tree_store_clear (GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list")));
            
            // no other model selected
            Preview->setCurModel (NULL);
            Preview->setCurShape (NULL);
            
            setActive ("add_shape", false);
            setActive ("remove_shape", false);
            setActive ("remove_sprite", false);            
        }
    }
}

// add shape to selected model
void GuiModeller::addShape ()
{
    GtkTreeIter iter;
    GtkTreeIter root;

    GtkTreeStore *shape_store = GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list"));

    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL(shape_store), &root))
    {
        world::placeable_shape *shape = NULL;
        gtk_tree_model_get (GTK_TREE_MODEL(shape_store), &root, 1, &shape, -1);            
        if (shape != NULL)
        {
            // TODO: set cube extension to sprite area + some height
            world::cube3 *cube = new world::cube3 (100, 100, 100);

            shape->add_part (cube);
            
            gtk_tree_store_append (shape_store, &iter, &root);
            gtk_tree_store_set (shape_store, &iter, 0, "cube", 1, (gpointer) cube, -1);
            
            // select newly added shape
            GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "shape_view"));
            GtkTreePath *path = gtk_tree_model_get_path (GTK_TREE_MODEL(shape_store), &iter);
            gtk_tree_view_expand_to_path (tree_view, path);
            
            GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
            gtk_tree_selection_select_path (selection, path);
            
            gtk_tree_path_free (path);
        }
    }
}

// remove the selected shape
void GuiModeller::removeShape ()
{
    GtkTreeIter parent, iter;
    GtkTreeModel *tree_model;
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "shape_view"));
    GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
    
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {
        world::cube3 *cube = NULL;
        world::placeable_shape *shape = NULL;
        
        // get selected shape
        if (gtk_tree_model_iter_parent (tree_model, &parent, &iter))
        {
            gtk_tree_model_get (tree_model, &parent, 1, &shape, -1);
        }

        // get selected cube
        gtk_tree_model_get (tree_model, &iter, 1, &cube, -1);
        if (shape != NULL && cube != NULL)
        {
            // remove shape from model
            shape->remove_part (cube);
            delete cube;
            
            // remove shape from interface
            if (!gtk_tree_store_remove (GTK_TREE_STORE(tree_model), &iter))
            {
                // no more shape selected
                Preview->setCurShape (NULL);
                setActive ("remove_shape", false);
            }
            else
            {
                // select new row
                GtkTreePath *path = gtk_tree_model_get_path (tree_model, &iter);
                gtk_tree_selection_select_path (selection, path);                
                gtk_tree_path_free (path);                
            }
        }
    }
}

// display model in shape list
void GuiModeller::updateShapeList (world::placeable_model *model)
{
    // set model in preview
    Preview->setCurModel (model);

    GtkTreeIter anim_iter;
    GtkTreeIter shape_iter;

    // set shape(s) in shape_list
    GtkTreeStore *shape_store = GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list"));

    // remove previous contents
    gtk_tree_store_clear (shape_store);

    world::placeable_shape *shape = model->current_shape ();
    if (shape == NULL)
    {
        // TODO: message to statusbar
        fprintf (stderr, "*** error: no current shape set in selected model!\n");
        return;
    }

    // set animation name as root of shape list
    std::string cur_anim = model->current_shape_name();
    gtk_tree_store_append (shape_store, &anim_iter, NULL);
    gtk_tree_store_set (shape_store, &anim_iter, 0, cur_anim.c_str(), 1, (gpointer) shape, -1);
    
    // add all existing shapes (if any)
    for (std::vector<world::cube3*>::const_iterator i = shape->begin(); i != shape->end(); i++)
    {
        gtk_tree_store_append (shape_store, &shape_iter, &anim_iter);
        gtk_tree_store_set (shape_store, &shape_iter, 0, "cube", 1, (gpointer) *i, -1);
    }
    
    // enable add shape button
    setActive ("add_shape", true);
    
    // TODO: update solid flag and image offset (X and Y only)
}

// enable or disable a widget
void GuiModeller::setActive (const std::string & id, const bool & sensitive)
{
    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (Ui, id.c_str()));
    gtk_widget_set_sensitive (widget, sensitive);
}

// try to get relative sprite path
bool GuiModeller::getRelativeSpritePath (std::string & sprite_path, std::string & base_path)
{
    // make canonical base path
    char canonical_path[PATH_MAX];
    if (realpath(base_path.c_str(), canonical_path))
    {
        base_path = canonical_path;
        if (realpath(sprite_path.c_str(), canonical_path))
        {
            sprite_path = canonical_path;
            if (sprite_path.compare (0, base_path.size(), base_path) == 0)
            {
                sprite_path = sprite_path.substr (base_path.length());
                if (sprite_path[0] == '/' || sprite_path[0] == '\\')
                {
                    sprite_path = sprite_path.substr (1);
                    return true;
                }
            }
        }
    }
    
    return false;
}
