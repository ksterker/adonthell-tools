/* 
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
 * @file mapedit/gui_entity_dialog.cc
 *
 * @author Kai Sterker
 * @brief View and edit entity properties.
 */

#include <gtk/gtk.h>

#include "gui_mapedit.h"
#include "gui_entity_dialog.h"

// Ui definition
static char edit_entity_ui[] =
    "<?xml version=\"1.0\"?>"
    "<interface>"
      "<!-- interface-requires gtk+ 2.12 -->"
      "<!-- interface-naming-policy toplevel-contextual -->"
      "<object class=\"GtkListStore\" id=\"entity_tags\">"
        "<columns>"
          "<!-- column-name gchararray1 -->"
          "<column type=\"gchararray\"/>"
        "</columns>"
      "</object>"
      "<object class=\"GtkDialog\" id=\"entity_properties\">"
        "<property name=\"border_width\">5</property>"
        "<property name=\"title\" translatable=\"yes\">Edit Entity</property>"
        "<property name=\"resizable\">False</property>"
        "<property name=\"modal\">True</property>"
        "<property name=\"window_position\">center</property>"
        "<property name=\"default_width\">400</property>"
        "<property name=\"default_height\">320</property>"
        "<property name=\"type_hint\">normal</property>"
        "<property name=\"has_separator\">False</property>"
        "<child internal-child=\"vbox\">"
          "<object class=\"GtkVBox\" id=\"dialog-vbox1\">"
            "<property name=\"visible\">True</property>"
            "<property name=\"orientation\">vertical</property>"
            "<property name=\"spacing\">2</property>"
            "<child>"
              "<object class=\"GtkNotebook\" id=\"notebook1\">"
                "<property name=\"visible\">True</property>"
                "<property name=\"can_focus\">True</property>"
                "<child>"
                  "<object class=\"GtkVBox\" id=\"vbox1\">"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"orientation\">vertical</property>"
                    "<child>"
                      "<object class=\"GtkFrame\" id=\"frame2\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"label_xalign\">0</property>"
                        "<child>"
                          "<object class=\"GtkAlignment\" id=\"alignment3\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"left_padding\">12</property>"
                            "<child>"
                              "<object class=\"GtkHBox\" id=\"hbox2\">"
                                "<property name=\"visible\">True</property>"
                                "<child>"
                                  "<object class=\"GtkImage\" id=\"img_preview\">"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"stock\">gtk-missing-image</property>"
                                    "<property name=\"icon-size\">6</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"position\">0</property>"
                                    "<property name=\"expand\">False</property>"
                                    "<property name=\"fill\">False</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkLabel\" id=\"lbl_name\">"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"xalign\">0</property>"
                                    "<property name=\"xpad\">10</property>"
                                    "<property name=\"label\" translatable=\"yes\">label</property>"
                                    "<property name=\"selectable\">True</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"position\">1</property>"
                                  "</packing>"
                                "</child>"
                              "</object>"
                            "</child>"
                          "</object>"
                        "</child>"
                        "<child type=\"label\">"
                          "<object class=\"GtkLabel\" id=\"label3\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"label\" translatable=\"yes\">&lt;b&gt;Overview&lt;/b&gt;</property>"
                            "<property name=\"use_markup\">True</property>"
                          "</object>"
                        "</child>"
                      "</object>"
                      "<packing>"
                        "<property name=\"position\">0</property>"
                      "</packing>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkFrame\" id=\"frame1\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"label_xalign\">0</property>"
                        "<child>"
                          "<object class=\"GtkAlignment\" id=\"alignment1\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"left_padding\">12</property>"
                            "<child>"
                              "<object class=\"GtkHBox\" id=\"hbox1\">"
                                "<property name=\"visible\">True</property>"
                                "<child>"
                                  "<object class=\"GtkRadioButton\" id=\"type_scenery\">"
                                    "<property name=\"label\" translatable=\"yes\">Scenery</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">False</property>"
                                    "<property name=\"tooltip_text\" translatable=\"yes\">These are static objects that usually cannot be interacted with.</property>"
                                    "<property name=\"active\">True</property>"
                                    "<property name=\"draw_indicator\">True</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"position\">0</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkRadioButton\" id=\"type_character\">"
                                    "<property name=\"label\" translatable=\"yes\">Character</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">False</property>"
                                    "<property name=\"tooltip_text\" translatable=\"yes\">These are mobile objects like NPCs and creatures.</property>"
                                    "<property name=\"active\">True</property>"
                                    "<property name=\"draw_indicator\">True</property>"
                                    "<property name=\"group\">type_scenery</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"position\">1</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkRadioButton\" id=\"type_item\">"
                                    "<property name=\"label\" translatable=\"yes\">Item</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">False</property>"
                                    "<property name=\"tooltip_text\" translatable=\"yes\">Objects that can be interacted with.</property>"
                                    "<property name=\"active\">True</property>"
                                    "<property name=\"draw_indicator\">True</property>"
                                    "<property name=\"group\">type_scenery</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"position\">2</property>"
                                  "</packing>"
                                "</child>"
                              "</object>"
                            "</child>"
                          "</object>"
                        "</child>"
                        "<child type=\"label\">"
                          "<object class=\"GtkLabel\" id=\"frame_type\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"label\" translatable=\"yes\">&lt;b&gt;Type&lt;/b&gt;</property>"
                            "<property name=\"use_markup\">True</property>"
                          "</object>"
                        "</child>"
                      "</object>"
                      "<packing>"
                        "<property name=\"position\">1</property>"
                      "</packing>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkFrame\" id=\"frame_entity\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"label_xalign\">0</property>"
                        "<child>"
                          "<object class=\"GtkAlignment\" id=\"alignment2\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"left_padding\">12</property>"
                            "<child>"
                              "<object class=\"GtkTable\" id=\"table1\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"n_rows\">2</property>"
                                "<property name=\"n_columns\">3</property>"
                                "<child>"
                                  "<object class=\"GtkEntry\" id=\"entity_id\">"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"tooltip_text\" translatable=\"yes\">The unique Id for Shared and Unique entities. Scripts can refer to entities via that Id.</property>"
                                    "<property name=\"invisible_char\">&#x25CF;</property>"
                                    "<property name=\"overwrite_mode\">True</property>"
                                    "<property name=\"activates-default\">True</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"left_attach\">1</property>"
                                    "<property name=\"right_attach\">3</property>"
                                    "<property name=\"top_attach\">1</property>"
                                    "<property name=\"bottom_attach\">2</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkRadioButton\" id=\"entity_anonymous\">"
                                    "<property name=\"label\" translatable=\"yes\">Anonymous</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">False</property>"
                                    "<property name=\"tooltip_text\" translatable=\"yes\">Objects that cannot be referred to in scripts. They all share the same sprite.</property>"
                                    "<property name=\"active\">True</property>"
                                    "<property name=\"draw_indicator\">True</property>"
                                  "</object>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkRadioButton\" id=\"entity_shared\">"
                                    "<property name=\"label\" translatable=\"yes\">Shared</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">False</property>"
                                    "<property name=\"tooltip_text\" translatable=\"yes\">Objects that can be referred to in scripts via their Id, but which still share the same sprite.</property>"
                                    "<property name=\"active\">True</property>"
                                    "<property name=\"draw_indicator\">True</property>"
                                    "<property name=\"group\">entity_anonymous</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"left_attach\">1</property>"
                                    "<property name=\"right_attach\">2</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkRadioButton\" id=\"entity_unique\">"
                                    "<property name=\"label\" translatable=\"yes\">Unique</property>"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"can_focus\">True</property>"
                                    "<property name=\"receives_default\">False</property>"
                                    "<property name=\"tooltip_text\" translatable=\"yes\">Objects that can be referred to in scripts via their Id. Each of them has its own sprite instance.</property>"
                                    "<property name=\"active\">True</property>"
                                    "<property name=\"draw_indicator\">True</property>"
                                    "<property name=\"group\">entity_anonymous</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"left_attach\">2</property>"
                                    "<property name=\"right_attach\">3</property>"
                                  "</packing>"
                                "</child>"
                                "<child>"
                                  "<object class=\"GtkLabel\" id=\"label2\">"
                                    "<property name=\"visible\">True</property>"
                                    "<property name=\"xalign\">1</property>"
                                    "<property name=\"xpad\">10</property>"
                                    "<property name=\"label\" translatable=\"yes\">Id:</property>"
                                    "<property name=\"justify\">right</property>"
                                  "</object>"
                                  "<packing>"
                                    "<property name=\"top_attach\">1</property>"
                                    "<property name=\"bottom_attach\">2</property>"
                                    "<property name=\"y_padding\">5</property>"
                                  "</packing>"
                                "</child>"
                              "</object>"
                            "</child>"
                          "</object>"
                        "</child>"
                        "<child type=\"label\">"
                          "<object class=\"GtkLabel\" id=\"label1\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"label\" translatable=\"yes\">&lt;b&gt;Entity&lt;/b&gt;</property>"
                            "<property name=\"use_markup\">True</property>"
                          "</object>"
                        "</child>"
                      "</object>"
                      "<packing>"
                        "<property name=\"position\">2</property>"
                      "</packing>"
                    "</child>"
                  "</object>"
                "</child>"
                "<child type=\"tab\">"
                  "<object class=\"GtkLabel\" id=\"lbl_general\">"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"label\" translatable=\"yes\">General</property>"
                  "</object>"
                  "<packing>"
                    "<property name=\"tab_fill\">False</property>"
                  "</packing>"
                "</child>"
                "<child>"
                  "<object class=\"GtkHBox\" id=\"hbox3\">"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"spacing\">4</property>"
                    "<child>"
                      "<object class=\"GtkVBox\" id=\"vbox2\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"orientation\">vertical</property>"
                        "<child>"
                          "<object class=\"GtkEntry\" id=\"new_tag\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"can_focus\">True</property>"
                            "<property name=\"invisible_char\">&#x25CF;</property>"
                          "</object>"
                          "<packing>"
                            "<property name=\"expand\">False</property>"
                            "<property name=\"padding\">4</property>"
                            "<property name=\"position\">0</property>"
                          "</packing>"
                        "</child>"
                        "<child>"
                          "<object class=\"GtkScrolledWindow\" id=\"scrolledwindow1\">"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"can_focus\">True</property>"
                            "<property name=\"hscrollbar_policy\">never</property>"
                            "<property name=\"shadow_type\">in</property>"
                            "<child>"
                              "<object class=\"GtkTreeView\" id=\"existing_tags\">"
                                "<property name=\"visible\">True</property>"
                                "<property name=\"can_focus\">True</property>"
                                "<property name=\"model\">entity_tags</property>"
                              "</object>"
                            "</child>"
                          "</object>"
                          "<packing>"
                            "<property name=\"padding\">4</property>"
                            "<property name=\"position\">1</property>"
                          "</packing>"
                        "</child>"
                      "</object>"
                      "<packing>"
                        "<property name=\"position\">0</property>"
                      "</packing>"
                    "</child>"
                    "<child>"
                      "<object class=\"GtkVButtonBox\" id=\"vbuttonbox1\">"
                        "<property name=\"visible\">True</property>"
                        "<property name=\"spacing\">10</property>"
                        "<property name=\"layout_style\">start</property>"
                        "<child>"
                          "<object class=\"GtkButton\" id=\"btn_add_tag\">"
                            "<property name=\"label\">gtk-add</property>"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"can_focus\">True</property>"
                            "<property name=\"receives_default\">True</property>"
                            "<property name=\"tooltip_text\" translatable=\"yes\">Add a new tag for this entity</property>"
                            "<property name=\"use_stock\">True</property>"
                          "</object>"
                          "<packing>"
                            "<property name=\"expand\">False</property>"
                            "<property name=\"fill\">False</property>"
                            "<property name=\"position\">0</property>"
                          "</packing>"
                        "</child>"
                        "<child>"
                          "<object class=\"GtkButton\" id=\"btn_remove_tag\">"
                            "<property name=\"label\">gtk-remove</property>"
                            "<property name=\"visible\">True</property>"
                            "<property name=\"can_focus\">True</property>"
                            "<property name=\"receives_default\">True</property>"
                            "<property name=\"tooltip_text\" translatable=\"yes\">Remove selected tag from this entity.</property>"
                            "<property name=\"use_stock\">True</property>"
                          "</object>"
                          "<packing>"
                            "<property name=\"expand\">False</property>"
                            "<property name=\"fill\">False</property>"
                            "<property name=\"padding\">4</property>"
                            "<property name=\"position\">1</property>"
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
                "<child type=\"tab\">"
                  "<object class=\"GtkLabel\" id=\"lbl_tags\">"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"label\" translatable=\"yes\">Tags</property>"
                  "</object>"
                  "<packing>"
                    "<property name=\"position\">1</property>"
                    "<property name=\"tab_fill\">False</property>"
                  "</packing>"
                "</child>"
                "<child>"
                  "<placeholder/>"
                "</child>"
                "<child type=\"tab\">"
                  "<placeholder/>"
                "</child>"
              "</object>"
              "<packing>"
                "<property name=\"position\">1</property>"
              "</packing>"
            "</child>"
            "<child internal-child=\"action_area\">"
              "<object class=\"GtkHButtonBox\" id=\"dialog-action_area1\">"
                "<property name=\"visible\">True</property>"
                "<property name=\"layout_style\">end</property>"
                "<child>"
                  "<object class=\"GtkButton\" id=\"btn_goto\">"
                    "<property name=\"label\" translatable=\"yes\">Center On</property>"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"can_focus\">True</property>"
                    "<property name=\"receives_default\">True</property>"
                    "<property name=\"tooltip_text\" translatable=\"yes\">Center map on entity.</property>"
                  "</object>"
                  "<packing>"
                    "<property name=\"expand\">False</property>"
                    "<property name=\"fill\">False</property>"
                    "<property name=\"position\">0</property>"
                  "</packing>"
                "</child>"
                "<child>"
                  "<object class=\"GtkButton\" id=\"btn_okay\">"
                    "<property name=\"label\">gtk-ok</property>"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"can_focus\">True</property>"
                    "<property name=\"can_default\">True</property>"
                    "<property name=\"has_default\">True</property>"
                    "<property name=\"receives_default\">True</property>"
                    "<property name=\"tooltip_text\" translatable=\"yes\">Apply changes to entity.</property>"
                    "<property name=\"use_stock\">True</property>"
                  "</object>"
                  "<packing>"
                    "<property name=\"expand\">False</property>"
                    "<property name=\"fill\">False</property>"
                    "<property name=\"position\">2</property>"
                  "</packing>"
                "</child>"
                "<child>"
                  "<object class=\"GtkButton\" id=\"btn_cancel\">"
                    "<property name=\"label\">gtk-close</property>"
                    "<property name=\"visible\">True</property>"
                    "<property name=\"can_focus\">True</property>"
                    "<property name=\"receives_default\">True</property>"
                    "<property name=\"tooltip_text\" translatable=\"yes\">Close dialog and discard all changes.</property>"
                    "<property name=\"use_stock\">True</property>"
                    "<property name=\"focus_on_click\">False</property>"
                  "</object>"
                  "<packing>"
                    "<property name=\"expand\">False</property>"
                    "<property name=\"fill\">False</property>"
                    "<property name=\"position\">3</property>"
                  "</packing>"
                "</child>"
              "</object>"
              "<packing>"
                "<property name=\"expand\">False</property>"
                "<property name=\"pack_type\">end</property>"
                "<property name=\"position\">0</property>"
              "</packing>"
            "</child>"
          "</object>"
        "</child>"
        "<action-widgets>"
          "<action-widget response=\"0\">btn_goto</action-widget>"
          "<action-widget response=\"0\">btn_okay</action-widget>"
          "<action-widget response=\"0\">btn_cancel</action-widget>"
        "</action-widgets>"
      "</object>"
    "</interface>";


// close the dialog and keep all changes
static void on_ok_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiEntityDialog *dialog = (GuiEntityDialog *) user_data;
    
    dialog->applyChanges ();
    
    // clean up
    gtk_main_quit ();
}

// close dialog and dismiss all changes
static void on_cancel_button_pressed (GtkButton * button, gpointer user_data)
{
    // clean up
    gtk_main_quit ();
}

// the entity object type has changed
static void on_type_changed (GtkToggleButton * button, gpointer user_data)
{
    if (gtk_toggle_button_get_active (button))
    {
        GuiEntityDialog *dlg = (GuiEntityDialog *) user_data;
        const gchar *id = gtk_widget_get_name (GTK_WIDGET(button));
        if (strcmp (id, "type_scenery") == 0)
        {
            dlg->set_object_type (world::OBJECT);
        }
        else if (strcmp (id, "type_character") == 0)
        {
            dlg->set_object_type (world::CHARACTER);
        }
        else if (strcmp (id, "type_item") == 0)
        {
            dlg->set_object_type (world::ITEM);
        }
        else if (strcmp (id, "entity_anonymous") == 0)
        {
            dlg->set_entity_type ('A');
        }
        else if (strcmp (id, "entity_shared") == 0)
        {
            dlg->set_entity_type ('S');
        }
        else if (strcmp (id, "entity_unique") == 0)
        {
            dlg->set_entity_type ('U');
        }
    }
}

// ctor
GuiEntityDialog::GuiEntityDialog (MapEntity *entity, const GuiEntityDialog::Mode & mode) 
    : GuiModalDialog (GTK_WINDOW(GuiMapedit::window->getWindow()))
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    Entity = entity;
    
    // set defaults
    EntityType = 'A';
    ObjType = world::OBJECT;
    
	if (!gtk_builder_add_from_string(Ui, edit_entity_ui, -1, &err)) 
    {
        g_message ("building entity dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get reference to dialog window
    window = GTK_WIDGET (gtk_builder_get_object (Ui, "entity_properties"));
    switch (mode)
    {
        case READ_ONLY:
            gtk_window_set_title (GTK_WINDOW (window), "View Entity Properties");
            break;
        case ADD_ENTITY_TO_MAP:
            gtk_window_set_title (GTK_WINDOW (window), "Add new Entity to Map");
            break;
        case DUPLICATE_NAMED_ENTITY:
            gtk_window_set_title (GTK_WINDOW (window), "Chose unique id for Entity");
            break;
    }
    
    // setup callbacks
    widget = gtk_builder_get_object (Ui, "btn_okay");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_ok_button_pressed), this);
    gtk_widget_set_sensitive (GTK_WIDGET (widget), mode != READ_ONLY);
    widget = gtk_builder_get_object (Ui, "btn_cancel");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_cancel_button_pressed), this);
    
    widget = gtk_builder_get_object (Ui, "type_scenery");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "type_character");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "type_item");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);

    widget = gtk_builder_get_object (Ui, "entity_anonymous");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "entity_shared");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    widget = gtk_builder_get_object (Ui, "entity_unique");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_type_changed), this);
    
    // disable goto button (until we actually have a use for it)
    widget = gtk_builder_get_object (Ui, "btn_goto");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), FALSE);
    
    // set name
    gchar *str = entity->get_name();
    widget = gtk_builder_get_object (Ui, "lbl_name");
    gtk_label_set_text (GTK_LABEL (widget), str);
    g_free (str);
    
    // set id
    if (mode == DUPLICATE_NAMED_ENTITY)
    {
        str = entity->createNewId();
    }
    else
    {
        str = entity->get_id();
    }
    
    if (str != NULL)
    {
        widget = gtk_builder_get_object (Ui, "entity_id");
        
        if (mode == DUPLICATE_NAMED_ENTITY)
        {
            gtk_widget_grab_focus (GTK_WIDGET (widget));
        }
            
        gtk_entry_set_text (GTK_ENTRY (widget), str);
        g_free (str);
    }
    
    // set image
    GdkPixbuf *img = entity->get_icon (64);
    widget = gtk_builder_get_object (Ui, "img_preview");
    gtk_image_set_from_pixbuf (GTK_IMAGE (widget), img);
    g_object_unref (img);

    // set placeable type
    world::placeable_type obj_type = entity->get_object_type();
    switch (obj_type)
    {
        case world::OBJECT:
            widget = gtk_builder_get_object (Ui, "type_scenery");
            break;
        case world::CHARACTER:
            widget = gtk_builder_get_object (Ui, "type_character");
            break;
        case world::ITEM:
            widget = gtk_builder_get_object (Ui, "type_item");
            break;
        default:
            widget = NULL;
            break;
    }
    if (widget != NULL)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
    }
    
    // set entity type
    str = entity->get_entity_type ();
    switch (str[0])
    {
        case 'A':
            widget = gtk_builder_get_object (Ui, "entity_anonymous");
            break;
        case 'S':
            widget = gtk_builder_get_object (Ui, "entity_shared");
            break;
        case 'U':
            widget = gtk_builder_get_object (Ui, "entity_unique");
            break;
        default:
            widget = NULL;
            break;
    }
    if (widget != NULL)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (widget), TRUE);
    }
    g_free (str);
}

// dtor
GuiEntityDialog::~GuiEntityDialog()
{
    // cleanup
    g_object_unref (Ui);
}

// "make it so!"
void GuiEntityDialog::applyChanges()
{
    const gchar* id = "";
    // get id, if neccessary
    if (EntityType == 'S' || EntityType == 'U')
    {
        GObject *widget = gtk_builder_get_object (Ui, "entity_id");
        id = gtk_entry_get_text (GTK_ENTRY (widget));
    }
    
    bool result = Entity->update_entity (ObjType, EntityType, id);
    okButtonPressed (result);
}

// update the object type
void GuiEntityDialog::set_object_type (const world::placeable_type & type)
{
    ObjType = type;
    
    GObject *cb_anonymous = gtk_builder_get_object (Ui, "entity_anonymous");
    GObject *cb_shared = gtk_builder_get_object (Ui, "entity_shared");
    GObject *cb_unique = gtk_builder_get_object (Ui, "entity_unique");
        
    // depending on the placeable type, only a number of entity types make sense
    switch (type)
    {
        case world::OBJECT:
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            break;
        case world::CHARACTER:
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            // since this is the only possibly choice --> chose it
            gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cb_unique), TRUE);            
            break;
        case world::ITEM:
            gtk_widget_set_sensitive (GTK_WIDGET (cb_anonymous), FALSE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_shared), TRUE);
            gtk_widget_set_sensitive (GTK_WIDGET (cb_unique), TRUE);
            // check if we have a valid choice --> if not chose a sensible default
            if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (cb_anonymous)))
            {
                gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (cb_shared), TRUE);            
            }
            break;
        default:
            break;
    }    
}

// update the entity type
void GuiEntityDialog::set_entity_type (const char & type)
{
    EntityType = type;

    // disable id field for anonymous entities ...
    GObject *widget = gtk_builder_get_object (Ui, "entity_id");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), type != 'A');
    // ... and grab focus for the rest
    if (type != 'A') gtk_widget_grab_focus (GTK_WIDGET (widget));
}
