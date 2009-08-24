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

#include "world/placeable_model.h"

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
                      "<object class=\"GtkImageMenuItem\" id=\"imagemenuitem1\">"
                        "<property name=\"label\">gtk-new</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"imagemenuitem2\">"
                        "<property name=\"label\">gtk-open</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"imagemenuitem3\">"
                        "<property name=\"label\">gtk-save</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
                      "</object>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkImageMenuItem\" id=\"imagemenuitem4\">"
                        "<property name=\"label\">gtk-save-as</property>"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"use_underline\">True</property>"
                        "<property name=\"use_stock\">True</property>"
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
                            "<child>"
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
                            "</child>"
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
"</interface>";

// Main Window: on_widget_destroy App
static void on_widget_destroy (GtkWidget * widget, gpointer data)
{
    gtk_main_quit ();
    gtk_widget_destroy (widget);
}

// callback for selection changes in sprite list
static void anim_selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *tree_model;
    GtkTreeIter iter;
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {
        // have we selected a leaf?
        if (!gtk_tree_model_iter_has_child (tree_model, &iter))
        {
            gchar *anim_name;
            gpointer data = NULL;

            // TODO: disable del_sprite button

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
                
                // and display it in editor
                GuiModeller *modeller = (GuiModeller *) user_data;
                modeller->updateShapeList (model);
            }
            
            // cleanup
            g_free (anim_name);
        }
        else
        {
            // TODO: activate del_sprite button
        }
    }
}

// shape selected in the shape list
static void shape_selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *tree_model;
    GtkTreeIter iter;
    
    // anything selected at all? 
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {
        // have we selected a leaf?
        if (!gtk_tree_model_iter_has_child (tree_model, &iter))
        {
            // TODO: enable remove_shape button
            
            world::cube3 *cube = NULL;
            gtk_tree_model_get (tree_model, &iter, 1, &cube, -1);   
            if (cube != NULL)
            {
                // and display it in editor
                GuiModeller *modeller = (GuiModeller *) user_data;
                modeller->getPreview ()->setCurShape (cube);
            }
        }
        else
        {
            // TODO: disable remove_shape button
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

static void on_add_shape_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->addShape ();
}


// ctor
GuiModeller::GuiModeller ()
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    
	if (!gtk_builder_add_from_string(Ui, modeller_ui, -1, &err)) 
    {
        g_message ("building modeller main window failed: %s", err->message);
        g_error_free (err);
        return;
    }

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

    // setup preview
    widget = gtk_builder_get_object (Ui, "model_area");
    Preview = new GuiPreview (GTK_WIDGET (widget));
        
    // connect signals
    widget = gtk_builder_get_object (Ui, "add_sprite");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_sprite_pressed), this);
    widget = gtk_builder_get_object (Ui, "add_shape");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_shape_pressed), this);
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

// add a new sprite to the model
void GuiModeller::addSprite (const std::string & name)
{
    // remember path to sprite for convenience
    gchar* sprite_path = g_path_get_dirname (name.c_str());
    SpriteDir = sprite_path;
    g_free (sprite_path);

    // create new model
    world::placeable_model *model = new world::placeable_model();
    model->set_sprite (name);
    
    // try loading sprite
    gfx::sprite *sprt = model->get_sprite ();
    
    if (sprt != NULL)
    {
        gchar *sprite_name = g_path_get_basename (name.c_str());
        
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
    
    // TODO: enable add shape button
    // TODO: update solid flag and image offset (X and Y only)
}
