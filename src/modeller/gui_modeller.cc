/*
 Copyright (C) 2009/2010 Kai Sterker <kaisterker@linuxgames.com>
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
#include <gdk/gdkkeysyms.h>

#ifdef MAC_INTEGRATION
#include <ige-mac-integration.h>
#endif

#include <base/base.h>
#include <base/diskio.h>
#include <world/placeable_model.h>

#include "common/util.h"

#include "mdl_cmdline.h"
#include "mdl_bbox_editor.h"
#include "mdl_point_editor.h"
#include "gui_connectors.h"
#include "gui_modeller.h"
#include "gui_preview.h"
#include "gui_file.h"

// Ui definition
static char modeller_ui[] =
#include "modeller.glade.h"
;

enum
{
    NAME_COLUMN,
    FACE_COLUMN,
    POS_COLUMN,
    NUM_COLUMNS
};

static void connector_list_tree_model_iface_init (GtkTreeModelIface *iface);
static int connector_list_get_n_columns (GtkTreeModel *self);
static GType connector_list_get_column_type (GtkTreeModel *self, int column);
static void connector_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value);

// ConnectorList inherits from GtkListStore, and implements the GtkTreeStore interface
G_DEFINE_TYPE_EXTENDED (ConnectorList, connector_list, GTK_TYPE_LIST_STORE, 0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, connector_list_tree_model_iface_init));

// our parent's model iface
static GtkTreeModelIface parent_iface = { { 0 }, };

// this method is called once to set up the class
static void connector_list_class_init (ConnectorListClass *klass)
{
}

// this method is called once to set up the interface
static void connector_list_tree_model_iface_init (GtkTreeModelIface *iface)
{
    // this is where we override the interface methods
    // first make a copy of our parent's interface to call later
    parent_iface = *iface;

    // now put in our own overriding methods
    iface->get_n_columns = connector_list_get_n_columns;
    iface->get_column_type = connector_list_get_column_type;
    iface->get_value = connector_list_get_value;
}

// this method is called every time an instance of the class is created
static void connector_list_init (ConnectorList *self)
{
    // initialise the underlying storage for the GtkListStore
    GType types[] = { G_TYPE_POINTER };

    gtk_list_store_set_column_types (GTK_LIST_STORE (self), 1, types);
}

// retrieve an object from our parent's data storage
static MdlConnector *connector_list_get_object (ConnectorList *self, GtkTreeIter *iter)
{
    GValue value = { 0, };
    MdlConnector *obj = NULL;

    // validate our parameters
    g_return_val_if_fail (IS_CONNECTOR_LIST (self), NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    // retreive the object using our parent's interface, take our own reference to it
    parent_iface.get_value (GTK_TREE_MODEL (self), iter, 0, &value);
    obj = (MdlConnector*) g_value_get_pointer (&value);

    // cleanup
    g_value_unset (&value);

    return obj;
}

// this method returns the number of columns in our tree model
static int connector_list_get_n_columns (GtkTreeModel *self)
{
    return NUM_COLUMNS;
}

// this method returns the type of each column in our tree model
static GType connector_list_get_column_type (GtkTreeModel *self, int column)
{
    static GType types[] = {
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT
    };

    // validate our parameters
    g_return_val_if_fail (IS_CONNECTOR_LIST (self), G_TYPE_INVALID);
    g_return_val_if_fail (column >= 0 && column < NUM_COLUMNS, G_TYPE_INVALID);

    return types[column];
}

// this method retrieves the value for a particular column
static void connector_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value)
{
    // validate our parameters
    g_return_if_fail (IS_CONNECTOR_LIST (self));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column >= 0 && column < NUM_COLUMNS);
    g_return_if_fail (value != NULL);

    // get the object from our parent's storage
    MdlConnector *obj = (MdlConnector*) connector_list_get_object (CONNECTOR_LIST (self), iter);

    // initialise our GValue to the required type
    g_value_init (value, connector_list_get_column_type (GTK_TREE_MODEL (self), column));

    switch (column)
    {
        case NAME_COLUMN:
        {
            const gchar *name = obj->name().c_str();
            g_value_set_string (value, name);
            break;
        }
        case FACE_COLUMN:
        {
            switch (obj->side())
            {
                case MdlConnector::FRONT:
                    g_value_set_string (value, "Front");
                    break;
                case MdlConnector::BACK:
                    g_value_set_string (value, "Back");
                    break;
                case MdlConnector::LEFT:
                    g_value_set_string (value, "Left");
                    break;
                case MdlConnector::RIGHT:
                    g_value_set_string (value, "Right");
                    break;
                default:
                    g_assert_not_reached ();
            }

            break;
        }
        case POS_COLUMN:
        {
            g_value_set_int (value, obj->pos ());
            break;
        }
        default:
        {
            g_assert_not_reached ();
        }
    }
}

/** supported editing modes */
enum
{
    BBOX_MODE,
    POINT_MODE
};

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
        directory = base::Paths().user_data_dir();
    }
    
    // open file chooser
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_OPEN, "Open Model", directory + "/");
    fs.add_filter ("*.amdl|*.xml", "Adonthell Model");
    fs.add_shortcut (base::Paths().user_data_dir() + "/models/");
    
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
    
    // generate save directory from sprite directory for new models
    if (filename.find ("untitled") != filename.npos)
    {        
        saveDir = modeller->spriteDirectory ();
        size_t index = saveDir.find ("/gfx");
        if (index != std::string::npos)
        {
            saveDir.replace (index, 4, "/models");
        }
        
        // use sprite name as a likely name for the new model
        filename = modeller->spritename();
        
        // filename might end in .png if we loaded a 'default' sprite
        if (filename.find (".png", filename.size() - 4) != std::string::npos ||
            filename.find (".xml", filename.size() - 4) != std::string::npos)
        {
            filename = filename.replace (filename.size() - 3, 3, "amdl");
        }
    }
    // otherwise save to directory the model came from
    else
    {
        saveDir = modeller->modelDirectory ();

        // encourage use of new-style model extension
        if (filename.find (".xml", filename.size() - 4) != std::string::npos)
        {
            filename = filename.replace (filename.size() - 3, 3, "amdl");
        }
    }
    
    // try to create directory, if it doesn't exist
    // TODO: make a program setting for that instead of doing it by default 
    g_mkdir_with_parents (saveDir.c_str(), 0755);
        
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_SAVE, "Save Model", saveDir + "/" + filename);
    fs.add_filter ("*.amdl", "Adonthell Model");
    fs.add_shortcut (base::Paths().user_data_dir() + "/models/");

    // File selection closed with OK
    if (fs.run ())
    {
        modeller->saveModel (fs.getSelection ());
        modeller->saveMeta (fs.getSelection ());
    }
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
        modeller->saveMeta (dirname + "/" + filename);
    }
    else
    {
        // otherwise open file selection
        on_file_save_as_activate (menuitem, user_data);
    }
}

// Model Menu: Zoom In
static void on_model_zoom_in (GtkMenuItem * menuitem, gpointer user_data)
{
    base::Scale++;

    GuiModeller *modeller = (GuiModeller *) user_data;

    modeller->setActive("item_zoom_out", true);
    modeller->setActive("item_zoom_in", base::Scale < 16);

    modeller->zoom();
}

// Model Menu: Zoom Out
static void on_model_zoom_out (GtkMenuItem * menuitem, gpointer user_data)
{
    base::Scale--;

    GuiModeller *modeller = (GuiModeller *) user_data;

    modeller->setActive("item_zoom_out", base::Scale > 1);
    modeller->setActive("item_zoom_in", true);

    modeller->zoom();
}

// Model Menu: Zoom Normal
static void on_model_reset_zoom (GtkMenuItem * menuitem, gpointer user_data)
{
    base::Scale = 1;

    GuiModeller *modeller = (GuiModeller *) user_data;

    modeller->setActive("item_zoom_out", false);
    modeller->setActive("item_zoom_in", true);

    modeller->zoom();
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
            
            // activate paste_shape button if there is a shape to paste
            modeller->setActive ("paste_shape", modeller->shapeInClipboard());

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
            // de-activate paste shape button
            modeller->setActive ("paste_shape", false);
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

// entry selected in the connector list
static void connector_selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;

    GObject *widget = modeller->getWidget("btn_remove_connector");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), gtk_tree_selection_count_selected_rows (selection));

    GtkTreeModel *tree_model;
    GtkTreeIter iter;

    // anything selected at all?
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {
        // display it in editor
        MdlConnector *connector = connector_list_get_object (CONNECTOR_LIST(tree_model), &iter);
        modeller->getPreview ()->setCurConnector (connector);
    }
    else
    {
        // clear previous connector, if any
        modeller->getPreview ()->setCurConnector (NULL);
    }
}

// entry selected in the tag list
static void tag_selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GtkBuilder *ui = (GtkBuilder *) user_data;

    GObject *widget = gtk_builder_get_object (ui, "btn_remove_tag");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), gtk_tree_selection_count_selected_rows (selection));
}

// load sprite from file
static void on_add_sprite_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    GtkWindow *parent = GTK_WINDOW(modeller->getWindow());
    
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_OPEN, "Load Sprite", modeller->spriteDirectory () + "/");
    fs.add_filter ("*.xml|*.png", "Adonthell Sprite");
    fs.add_shortcut (base::Paths().user_data_dir() + "/gfx/");

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

// copy shapes to clipboard
static void on_copy_shape_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->copyShapes ();
}

// paste shapes from clipboard
static void on_paste_shape_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->pasteShapes ();
}

// remove shape from model
static void on_remove_shape_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->removeShape ();
}

// add connector to model
static void on_add_connector_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->addConnector();
}

// remove connector from model
static void on_remove_connector_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->removeConnector();
}

// add tag to model
static void on_add_tag_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->addTag();
}

// remove tag from model
static void on_remove_tag_pressed (GtkButton * button, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->removeTag();
}

// connector edited
static void connector_edited_event (GtkCellRendererText *cell, gchar *path, gchar *new_val, gpointer user_data)
{
    GtkTreeIter iter;
    GuiModeller *modeller = (GuiModeller *) user_data;
    GtkTreeModel *model = modeller->getConnectors();

    if (gtk_tree_model_get_iter_from_string (model, &iter, path))
    {
        MdlConnector *connector = connector_list_get_object(CONNECTOR_LIST(model), &iter);

        guint col = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "column_index"));
        switch (col)
        {
            case FACE_COLUMN:
            {
                MdlConnector::face side;
                switch (new_val[0])
                {
                    case 'B':
                        side = MdlConnector::BACK;
                        break;
                    case 'F':
                        side = MdlConnector::FRONT;
                        break;
                    case 'R':
                        side = MdlConnector::RIGHT;
                        break;
                    default:
                        side = MdlConnector::LEFT;
                        break;
                }

                modeller->updateConnectorFace(connector, side);
                break;
            }
            case POS_COLUMN:
            {
                s_int16 pos = (s_int16) atoi(new_val);

                modeller->updateConnectorPos(connector, pos);
                break;
            }
        }
    }
}

// changed solid state of shape
static void on_solid_state_changed (GtkToggleButton *togglebutton, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->setSolid (gtk_toggle_button_get_active (togglebutton));
}

// update "add_tag" button state
static void on_tag_entry_changed (GtkEditable *editable, gpointer user_data)
{
    GObject *widget;
    GtkBuilder *ui = (GtkBuilder*) user_data;

    // check for valid tag (TODO: prevent duplicates)
    widget = gtk_builder_get_object (ui, "entry_new_tag");
    bool enable = gtk_entry_get_text_length (GTK_ENTRY(widget)) > 0;

    // enable or disable "add tag" button
    widget = gtk_builder_get_object (ui, "btn_add_tag");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), enable);
}

// changed edit mode
static void on_edit_mode_changed (GtkToggleButton *togglebutton, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    if (gtk_toggle_button_get_active (togglebutton))
    {
        g_object_set (GTK_WIDGET(togglebutton),
                "label", "Point Mode",
                "tooltip-text", "In this mode, the position of each corner point can be edited. Click to switch to BBox Mode.", NULL);
        modeller->setEditingMode(POINT_MODE);
    }
    else
    {
        g_object_set (GTK_WIDGET(togglebutton),
                "label", "BBox Mode",
                "tooltip-text", "In this mode, the general size and position of the selected shape can be edited. Click to switch to Point Mode.", NULL);
        modeller->setEditingMode(BBOX_MODE);
    }
}

//
static void on_tab_switched (GtkNotebook *nb, gpointer page, guint page_num, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    switch (page_num)
    {
        // appearance
        case 0:
        {
            GtkTreeView *sprite_view = GTK_TREE_VIEW (modeller->getWidget("sprite_view"));
            anim_selected_event (gtk_tree_view_get_selection (sprite_view), user_data);
            break;
        }
        // meta-data
        case 1:
        {
            modeller->getPreview()->setCurShape(NULL);
            modeller->getPreview()->setCurModel(NULL);

            GtkTreeView *connector_view = GTK_TREE_VIEW (modeller->getWidget("view_connectors"));
            connector_selected_event (gtk_tree_view_get_selection (connector_view), user_data);

            break;
        }
    }
}

// ctor
GuiModeller::GuiModeller ()
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    Filename = "untitled.amdl";
    SpriteDir = MdlCmdline::datadir + "/" + MdlCmdline::project;
    Spritename = "";
    ShapeToPaste = NULL;

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
    GtkTreeModel *sprites = GTK_TREE_MODEL(gtk_builder_get_object (Ui, "sprite_list"));
    Preview = new GuiPreview (GTK_WIDGET (widget), shapeData, sprites);
    setEditingMode(BBOX_MODE);
    
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
    widget = gtk_builder_get_object (Ui, "item_zoom_in");
    g_signal_connect (widget, "activate", G_CALLBACK (on_model_zoom_in), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_zoom_out");
    g_signal_connect (widget, "activate", G_CALLBACK (on_model_zoom_out), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "item_zoom_normal");
    g_signal_connect (widget, "activate", G_CALLBACK (on_model_reset_zoom), (gpointer) this);

    // connect button signals on appearance pane
    widget = gtk_builder_get_object (Ui, "add_sprite");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_sprite_pressed), this);
    widget = gtk_builder_get_object (Ui, "remove_sprite");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_remove_sprite_pressed), this);
    widget = gtk_builder_get_object (Ui, "add_shape");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_shape_pressed), this);
    widget = gtk_builder_get_object (Ui, "copy_shape");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_copy_shape_pressed), this);
    widget = gtk_builder_get_object (Ui, "paste_shape");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_paste_shape_pressed), this);
    widget = gtk_builder_get_object (Ui, "remove_shape");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_remove_shape_pressed), this);

    widget = gtk_builder_get_object (Ui, "is_solid");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_solid_state_changed), (gpointer) this);
    widget = gtk_builder_get_object (Ui, "btn_edit_mode");
    g_signal_connect (widget, "toggled", G_CALLBACK (on_edit_mode_changed), (gpointer) this);

    // buttons on meta data pane
    widget = gtk_builder_get_object (Ui, "btn_add_tag");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_tag_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_remove_tag");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_remove_tag_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_add_connector");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_add_connector_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_remove_connector");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_remove_connector_pressed), this);

    // callbacks to check for valid values in edit fields
    widget = gtk_builder_get_object (Ui, "entry_new_tag");
    g_signal_connect (widget, "changed", G_CALLBACK (on_tag_entry_changed), Ui);

    // callback when toggling between appearance and meta tab
    widget = gtk_builder_get_object (Ui, "notebook1");
    g_signal_connect (widget, "switch-page", G_CALLBACK (on_tab_switched), (gpointer) this);

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

    widget = gtk_builder_get_object (Ui, "view_tags");
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
    g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(tag_selected_event), Ui);

    // create the connector  model
    widget = gtk_builder_get_object (Ui, "view_connectors");
    GtkListStore *model = (GtkListStore *) g_object_new (TYPE_CONNECTOR_LIST, NULL);
    gtk_tree_view_set_model (GTK_TREE_VIEW(widget), (GtkTreeModel*) model);

    // add selection listener
    selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
    g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(connector_selected_event), this);

    // add cell edit listeners
    widget = gtk_builder_get_object (Ui, "rnd_connector_face");
    g_object_set_data (widget, "column_index", GUINT_TO_POINTER(FACE_COLUMN));
    g_signal_connect (G_OBJECT(widget), "edited", G_CALLBACK(connector_edited_event), this);
    widget = gtk_builder_get_object (Ui, "rnd_connector_pos");
    g_object_set_data (widget, "column_index", GUINT_TO_POINTER(POS_COLUMN));
    g_signal_connect (G_OBJECT(widget), "edited", G_CALLBACK(connector_edited_event), this);

    // can't zoom less than 1
    setActive("item_zoom_out", false);

    // update title
    setTitle(false);
}

// start creating a new model
void GuiModeller::newModel ()
{
    // cleanup any previous stuff
    gtk_tree_store_clear (GTK_TREE_STORE(gtk_builder_get_object (Ui, "sprite_list")));
    gtk_tree_store_clear (GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list")));

    gtk_list_store_clear (GTK_LIST_STORE(gtk_builder_get_object (Ui, "tag_list")));
    gtk_list_store_clear (GTK_LIST_STORE(getConnectors()));

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (gtk_builder_get_object (Ui, "entry_description")));
    gtk_text_buffer_set_text (buffer, "", 0);
    gtk_entry_set_text (GTK_ENTRY (gtk_builder_get_object (Ui, "entry_new_tag")), "");
    
    Preview->setCurModel (NULL);
    Preview->setCurShape (NULL);
    
    // reset state
    Filename = "untitled.amdl";
    Spritename = "";
    
    // purge gfx cache, in case images changed on disk
    // TODO: make sure all sprites are properly deleted first
    gfx::surfaces->purge();
    
    // set initial button state
    setActive("is_solid", false);
    setActive("add_shape", false);
    setActive("copy_shape", false);
    setActive("remove_shape", false);
    setActive("remove_sprite", false);
    setActive("btn_add_tag", false);
    setActive("btn_remove_tag", false);
    setActive("btn_remove_connector", false);

    // update title
    setTitle(false);
}

// load model from disk
void GuiModeller::loadModel (const std::string & name)
{
    // reset
    newModel();
    
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
    
    char *id;
    void *value;
    u_int32 size;
    
    // iterate over placeable object
    while (placeable.next (&value, &size, &id) != base::flat::T_UNKNOWN)
    {
        // found model data
        if (strcmp ("model", id) == 0)
        {            
            base::flat mdl ((const char*) value, size);
            
            // load model
            world::placeable_model * model = new world::placeable_model ();
            model->get_state (mdl);
            
            // and add it to the UI
            addModel (model);
        }
    }

    // load model meta data (if present)
    loadMeta (name);

    // update title
    setTitle(false);
}    

// load meta data
void GuiModeller::loadMeta (const std::string & name)
{
    size_t idx = name.find_last_of('.');
    if (idx == std::string::npos)
    {
        return;
    }

    std::string meta_file_name = name.substr(0, idx) + ".xtra";
    if (!base::Paths().find_in_path(meta_file_name, false))
    {
        return;
    }

    base::diskio meta_data (base::diskio::BY_EXTENSION);
    if (!meta_data.get_record (meta_file_name))
    {
        return;
    }

    // load description
    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (gtk_builder_get_object (Ui, "entry_description")));
    gtk_text_buffer_set_text (buffer, meta_data.get_string("dscr").c_str(), -1);

    // load tags
    void *value;
    base::flat tags = meta_data.get_flat("tags");
    while (tags.next(&value) != base::flat::T_UNKNOWN)
    {
        addTag(strdup ((gchar *) value));
    }

    // make sure the connector templates are up-to-date
    MdlConnectorManager::load(base::Paths().user_data_dir());

    // load connectors
    u_int32 size;
    base::flat connectors = meta_data.get_flat("ctrs");
    while (connectors.next(&value, &size) != base::flat::T_UNKNOWN)
    {
        base::flat connector ((char *) value, size);
        MdlConnectorTemplate *tmpl = MdlConnectorManager::get(connector.get_uint32("tmpl"));
        if (tmpl != NULL)
        {
            MdlConnector *ctor = new MdlConnector (tmpl);
            ctor->set_side((MdlConnector::face) connector.get_uint8 ("side"));
            ctor->set_pos(connector.get_sint16 ("pos"));

            addConnector(ctor);
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
    
    world::placeable_model *model = NULL;
    base::diskio placeable (base::diskio::XML_FILE);

    GtkTreeIter iter;
    GtkTreeModel *tree_model = GTK_TREE_MODEL(gtk_builder_get_object (Ui, "sprite_list"));
    
    // iterate over models in sprite_tree
    if (gtk_tree_model_get_iter_first (tree_model, &iter))
    {
        do
        {
            gtk_tree_model_get (tree_model, &iter, 1, &model, -1);
            if (model != NULL)
            {
                model->put_state (placeable);
            }
        }
        while (gtk_tree_model_iter_next (tree_model, &iter));
    }
    
    placeable.put_record (name);

    // update title
    setTitle(false);
}

// save model meta data
void GuiModeller::saveMeta (const std::string & name) const
{
    std::string meta_file_name;

    size_t idx = name.find_last_of('.');
    if (idx != std::string::npos)
    {
        base::diskio meta_data (base::diskio::XML_FILE);

        // save description
        GtkTextIter start, end;
        GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (gtk_builder_get_object (Ui, "entry_description")));
        gtk_text_buffer_get_bounds (buffer, &start, &end);
        std::string description (gtk_text_buffer_get_text(buffer, &start, &end, FALSE));
        meta_data.put_string("dscr", description);

        // save tags
        base::flat tags;
        GtkTreeIter iter;
        GtkTreeModel *tree_model = GTK_TREE_MODEL(gtk_builder_get_object (Ui, "tag_list"));
        if (gtk_tree_model_get_iter_first (tree_model, &iter))
        {
            gchar *tag;
            do
            {
                gtk_tree_model_get (tree_model, &iter, 0, &tag, -1);
                if (tag != NULL)
                {
                    tags.put_string ("", tag);
                }
            }
            while (gtk_tree_model_iter_next (tree_model, &iter));
        }
        meta_data.put_flat("tags", tags);

        // save connectors
        base::flat connectors;
        tree_model = getConnectors ();
        if (gtk_tree_model_get_iter_first (tree_model, &iter))
        {
            do
            {
                MdlConnector *connector = connector_list_get_object (CONNECTOR_LIST(tree_model), &iter);
                if (connector != NULL)
                {
                    base::flat ctor;

                    ctor.put_uint32 ("tmpl", connector->uid());
                    ctor.put_uint8 ("side", connector->side());
                    ctor.put_sint16 ("pos", connector->pos());

                    connectors.put_flat("", ctor);
                }
            }
            while (gtk_tree_model_iter_next (tree_model, &iter));
        }
        meta_data.put_flat("ctrs", connectors);

        // save file to disk
        meta_file_name = name.substr(0, idx) + ".xtra";
        meta_data.put_record (meta_file_name);
    }
}

// add a new sprite to the model
void GuiModeller::addSprite (const std::string & name)
{
    // remember path to sprite for convenience
    gchar* sprite_dir = g_path_get_dirname (name.c_str());
    SpriteDir = sprite_dir;
    g_free (sprite_dir);

    // remember name of first sprite as potential model name
    if (Spritename.length() == 0)
    {
        gchar* sprite_file = g_path_get_basename (name.c_str());
        Spritename = sprite_file;
        g_free (sprite_file);
    }
    
    // create new model
    world::placeable_model *model = new world::placeable_model();
    
    // try to create a relative sprite path
    std::string sprite_path = util::get_relative_path (name, "/gfx");
    if (g_path_is_absolute (sprite_path.c_str()))
    {
        // FIXME: display error in status bar
        printf ("*** warning: cannot create sprite path relative to data directory!\n");
    }
    
    // set relative sprite name
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

        // update modified state
        setTitle(true);
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
            
            // just a precaution, as it might have been part of the model we just deleted
            ShapeToPaste = NULL;
            
            // no other model selected
            Preview->setCurModel (NULL);
            Preview->setCurShape (NULL);
            
            setActive ("is_solid", false);
            setActive ("add_shape", false);
            setActive ("copy_shape", false);
            setActive ("paste_shape", false);
            setActive ("remove_shape", false);
            setActive ("remove_sprite", false);

            // update modified state
            setTitle(true);
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

            // update modified state
            setTitle(true);
        }
    }
}

// copy shapes to clipboard
void GuiModeller::copyShapes ()
{
    GtkTreeIter root;    
    GtkTreeStore *shape_store = GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list"));
    
    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL(shape_store), &root))
    {
        gtk_tree_model_get (GTK_TREE_MODEL(shape_store), &root, 1, &ShapeToPaste, -1);
    }
}

// paste shapes from clipboard
void GuiModeller::pasteShapes ()
{
    if (ShapeToPaste == NULL) return;
    
    GtkTreeIter iter;
    GtkTreeModel *tree_model;
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "sprite_view"));
    GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
    
    // anything selected at all?
    if (gtk_tree_selection_get_selected (selection, &tree_model, &iter))
    {
        // have we selected a leaf?
        if (!gtk_tree_model_iter_has_child (tree_model, &iter))
        {
            // get selected model
            GtkTreeIter parent;
            gtk_tree_model_iter_parent (tree_model, &parent, &iter);
            
            world::placeable_model *model;
            gtk_tree_model_get (tree_model, &parent, 1, &model, -1);            
            
            if (model != NULL)
            {
                // finally get selected shape
                world::placeable_shape *shape = model->current_shape();
                
                // selected shape different from pasted shape?
                if (shape == ShapeToPaste)
                {
                    return;
                }
                
                // clear current shape
                std::vector<world::cube3*>::const_iterator i;
                while ((i = shape->begin()) != shape->end())
                {
                    shape->remove_part (*i);
                    delete *i;
                }
                
                // clone pasted shape
                for (i = ShapeToPaste->begin(); i != ShapeToPaste->end(); i++)
                {
                    world::cube3 *part = new world::cube3 (0, 0, 0);
                    for (int j = 0; j < world::cube3::NUM_CORNERS; j++)
                    {
                        part->set_point (j, (*i)->get_point (j));
                    }
                    
                    part->create_bounding_box ();
                    shape->add_part (part);
                }
                
                // copy offset
                shape->set_offset (ShapeToPaste->ox(), ShapeToPaste->oy());
                
                // update preview
                updateShapeList (model);

                // update modified state
                setTitle(true);
            }                
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

            // update modified state
            setTitle(true);
        }
    }
}

// update the current shape's solid flag
void GuiModeller::setSolid (const bool & is_solid)
{
    GtkTreeIter root;
    
    GtkTreeStore *shape_store = GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list"));
    
    if (gtk_tree_model_get_iter_first (GTK_TREE_MODEL(shape_store), &root))
    {
        world::placeable_shape *shape = NULL;
        gtk_tree_model_get (GTK_TREE_MODEL(shape_store), &root, 1, &shape, -1);            
        if (shape != NULL)
        {
            shape->set_solid (is_solid);

            // update modified state
            setTitle(true);
        }
    }
}

// display model in shape list
void GuiModeller::updateShapeList (world::placeable_model *model)
{
    // set model in preview
    Preview->setCurModel (model);

    world::placeable_shape *shape = model->current_shape ();
    if (shape == NULL)
    {
        // TODO: message to statusbar
        fprintf (stderr, "*** error: no current shape set in selected model!\n");
        return;
    }

    GtkTreeIter anim_iter;
    GtkTreeIter shape_iter;

    // set shape(s) in shape_list
    GtkTreeStore *shape_store = GTK_TREE_STORE(gtk_builder_get_object (Ui, "shape_list"));

    // remove previous contents
    gtk_tree_store_clear (shape_store);

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
    
    // expand list
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "shape_view"));
    gtk_tree_view_expand_all (tree_view);

    // select first shape
    if (gtk_tree_model_iter_n_children (GTK_TREE_MODEL(shape_store), &anim_iter) > 0)
    {
        GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);
        gtk_tree_model_iter_nth_child (GTK_TREE_MODEL(shape_store), &shape_iter, &anim_iter, 0);
        gtk_tree_selection_select_iter (selection, &shape_iter);
    }

    // enable add shape button
    setActive ("copy_shape", true);
    setActive ("add_shape", true);
    setActive ("is_solid", true);
    
    // update solid flag
    GtkToggleButton *is_solid = GTK_TOGGLE_BUTTON(gtk_builder_get_object (Ui, "is_solid"));
    gtk_toggle_button_set_active (is_solid, shape->is_solid ());
}

// add a connector to the model
void GuiModeller::addConnector (MdlConnector *ctor)
{
    if (ctor == NULL)
    {
        GuiConnectors dlg(GTK_WINDOW (Window), Ui, Preview->modelLength(), Preview->modelWidth());
        if (dlg.run())
        {
            MdlConnectorTemplate *tmpl = dlg.selectedTemplate();
            ctor = new MdlConnector (tmpl);

            setTitle (true);
        }
    }

    if (ctor != NULL)
    {
        GtkTreeIter iter;
        GtkTreeModel *model = getConnectors ();

        gtk_list_store_append (GTK_LIST_STORE(model), &iter);
        gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, ctor, -1);
    }
}

// remove connector from the model
void GuiModeller::removeConnector ()
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "view_connectors"));
    GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        MdlConnector *connector = NULL;
        gtk_tree_model_get (model, &iter, 0, &connector, -1);
        if (connector != NULL)
        {
            gtk_list_store_remove (GTK_LIST_STORE(model), &iter);
            delete connector;

            setTitle (true);
        }
    }
}

// update side connector is attached to
void GuiModeller::updateConnectorFace (MdlConnector *connector, MdlConnector::face side)
{
    connector->set_side(side);
    Preview->render();
    setTitle (true);
}

// update position of connector
void GuiModeller::updateConnectorPos (MdlConnector *connector, const s_int16 & pos)
{
    connector->set_pos(pos);
    Preview->render();
    setTitle (true);
}

// get list of connectors
GtkTreeModel* GuiModeller::getConnectors() const
{
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "view_connectors"));
    return gtk_tree_view_get_model (tree_view);
}

// add tag
void GuiModeller::addTag (gchar *tag)
{
    if (tag == NULL)
    {
        GtkEntry *entry = GTK_ENTRY (gtk_builder_get_object (Ui, "entry_new_tag"));
        tag = strdup (gtk_entry_get_text (entry));

        gtk_entry_set_text (entry, "");
        gtk_widget_grab_focus(GTK_WIDGET(entry));

        setTitle (true);
    }

    if (tag != NULL)
    {
        GtkTreeIter iter;
        GtkTreeModel *model = GTK_TREE_MODEL(gtk_builder_get_object (Ui, "tag_list"));

        gtk_list_store_append (GTK_LIST_STORE(model), &iter);
        gtk_list_store_set (GTK_LIST_STORE(model), &iter, 0, tag, -1);
    }
}

// remove tag
void GuiModeller::removeTag()
{
    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "view_tags"));
    GtkTreeSelection *selection = gtk_tree_view_get_selection (tree_view);

    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        gchar *tag = NULL;
        gtk_tree_model_get (model, &iter, 0, &tag, -1);
        if (tag != NULL)
        {
            gtk_list_store_remove (GTK_LIST_STORE(model), &iter);
            setTitle (true);
        }
    }
}

// zoom displayed model
void GuiModeller::zoom()
{
    // adapt scrolled region when zoom level changes
    Preview->setScrollOffset(0, 0);
    Preview->scroll();

    // update display
    Preview->render();
}

// enable or disable a widget
void GuiModeller::setActive (const std::string & id, const bool & sensitive)
{
    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (Ui, id.c_str()));
    gtk_widget_set_sensitive (widget, sensitive);
}

void GuiModeller::setEditingMode (const int & editing_mode)
{
    static BboxEditor bbox_mode;
    static PointEditor point_mode;

    switch (editing_mode)
    {
        case BBOX_MODE:
        {
            Preview->setEditor (&bbox_mode);
            break;
        }
        case POINT_MODE:
        {
            Preview->setEditor (&point_mode);
            break;
        }
    }
}

// update title bar
void GuiModeller::setTitle(const bool & modified)
{
    std::string title = "Adonthell Modeller v"VERSION" [";
    title += filename();
    title += modified ? "*]" : "]";

    gtk_window_set_title(GTK_WINDOW(Window), title.c_str());
}
