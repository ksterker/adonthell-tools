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

#ifdef MAC_INTEGRATION
#include <ige-mac-integration.h>
#endif

#include <base/base.h>
#include <base/diskio.h>
#include <world/placeable_model.h>

#include "common/util.h"

#include "mdl_cmdline.h"
#include "gui_modeller.h"
#include "gui_preview.h"
#include "gui_file.h"

// Ui definition
static char modeller_ui[] =
#include "modeller.glade.h"
;

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
    fs.add_filter ("*.xml", "Adonthell Model");
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
        if (filename.find (".png", filename.size() - 4) != std::string::npos)
        {
            filename = filename.replace (filename.size() - 3, 3, "xml");
        }
    }
    // otherwise save to directory the model came from
    else
    {
        saveDir = modeller->modelDirectory ();
    }
    
    // try to create directory, if it doesn't exist
    // TODO: make a program setting for that instead of doing it by default 
    g_mkdir_with_parents (saveDir.c_str(), 0755);
        
    GuiFile fs (parent, GTK_FILE_CHOOSER_ACTION_SAVE, "Save Model", saveDir + "/" + filename);
    fs.add_filter ("*.xml", "Adonthell Model");
    fs.add_shortcut (base::Paths().user_data_dir() + "/models/");

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

// Model Menu: Zoom In
static void on_model_zoom_in (GtkMenuItem * menuitem, gpointer user_data)
{
    base::Scale++;

    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->setActive("item_zoom_out", true);
    modeller->zoom();
}

// Model Menu: Zoom Out
static void on_model_zoom_out (GtkMenuItem * menuitem, gpointer user_data)
{
    base::Scale--;

    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->setActive("item_zoom_out", base::Scale > 1);
    modeller->zoom();
}

// Model Menu: Zoom Normal
static void on_model_reset_zoom (GtkMenuItem * menuitem, gpointer user_data)
{
    base::Scale = 1;

    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->setActive("item_zoom_out", false);
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

// changed solid state of shape
static void on_solid_state_changed (GtkToggleButton *togglebutton, gpointer user_data)
{
    GuiModeller *modeller = (GuiModeller *) user_data;
    modeller->setSolid (gtk_toggle_button_get_active (togglebutton));
}

// ctor
GuiModeller::GuiModeller ()
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    Filename = "untitled.xml";
    SpriteDir = MdlCmdline::datadir + "/" + MdlCmdline::project;
    Spritename = "";
    
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

    // connect button signals
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
    
    Preview->setCurModel (NULL);
    Preview->setCurShape (NULL);
    
    // reset state
    Filename = "untitled.xml";
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

    // update title
    setTitle(false);
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
    base::diskio placeable (base::diskio::BY_EXTENSION);

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
    
    // expand list
    GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_builder_get_object (Ui, "shape_view"));
    gtk_tree_view_expand_all (tree_view);

    // enable add shape button
    setActive ("copy_shape", true);
    setActive ("add_shape", true);
    setActive ("is_solid", true);
    
    // update solid flag
    GtkToggleButton *is_solid = GTK_TOGGLE_BUTTON(gtk_builder_get_object (Ui, "is_solid"));
    gtk_toggle_button_set_active (is_solid, shape->is_solid ());
}

// zoom displayed model
void GuiModeller::zoom()
{
    Preview->draw(0, 0, 800, 600);
}

// enable or disable a widget
void GuiModeller::setActive (const std::string & id, const bool & sensitive)
{
    GtkWidget *widget = GTK_WIDGET(gtk_builder_get_object (Ui, id.c_str()));
    gtk_widget_set_sensitive (widget, sensitive);
}

// update title bar
void GuiModeller::setTitle(const bool & modified)
{
    std::string title = "Adonthell Modeller v"VERSION" [";
    title += filename();
    title += modified ? "*]" : "]";

    gtk_window_set_title(GTK_WINDOW(Window), title.c_str());
}
