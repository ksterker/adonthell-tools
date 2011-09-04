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
 * @file modeller/gui_modeller.h
 *
 * @author Kai Sterker
 * @brief The modeller main window.
 */


#ifndef GUI_MODELLER_H
#define GUI_MODELLER_H

#include "common/mdl_connector.h"

G_BEGIN_DECLS

#define TYPE_CONNECTOR_LIST    (connector_list_get_type ())
#define CONNECTOR_LIST(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_CONNECTOR_LIST, ConnectorList))
#define CONNECTOR_LIST_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), TYPE_CONNECTOR_LIST, ConnectorListClass))
#define IS_CONNECTOR_LIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_CONNECTOR_LIST))
#define IS_CONNECTOR_LIST_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE ((obj), TYPE_CONNECTOR_LIST))
#define CONNECTOR_LIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_CONNECTOR_LIST, ConnectorListClass))

typedef struct _ConnectorList ConnectorList;
typedef struct _ConnectorListClass ConnectorListClass;

struct _ConnectorList
{
    GtkListStore parent;
};

struct _ConnectorListClass
{
    GtkListStoreClass parent_class;
};

GType connector_list_get_type (void);

G_END_DECLS

namespace world
{
    class placeable_shape;
    class placeable_model;
};

class GuiPreview;

/**
 * The application main window.
 */
class GuiModeller
{
public:
    /**
     * Create new instance of the GUI
     */
    GuiModeller();
    
    /**
     * Get the application main window.
     * @return the main window.
     */
    GtkWidget *getWindow () const { return Window; }
    
    /**
     * Get pointer to the preview area.
     * @return the preview area.
     */
    GuiPreview *getPreview () const { return Preview; }

    /**
     * Get pointer to a widget of the given name.
     * @return widget of the given name.
     */
    GObject *getWidget (const std::string & name) const
    {
        gtk_builder_get_object (Ui, name.c_str());
    }
    
    /**
     * Get the tree model containing all connectors of the current model.
     * @return the connectors of the current model
     */
    GtkTreeModel *getConnectors () const;

    /**
     * Return the directory used for loading sprites.
     * @return the directory the last sprite was picked from.
     */
    std::string spriteDirectory () const { return SpriteDir; }

    /**
     * Return the directory used for loading/saving models.
     * @return the directory the last model was loaded from/saved to.
     */
    std::string modelDirectory () const { return ModelDir; }
    
    /**
     * Return the filename of the current model (without the path).
     * @return filename of the current model.
     */
    std::string filename () const { return Filename; }
    
    /**
     * Return the filename of the first sprite added to a new model.
     * @return filename of the first sprite added to the model.
     */
    std::string spritename () const { return Spritename; }
    
    /**
     * Start working on a new model. Clears all current data.
     */
    void newModel ();

    /**
     * Load the model from the given file.
     * @param name file to load the model from.
     */
    void loadModel (const std::string & name);    
    
    /**
     * Load the model meta data from the given file.
     * @param name file to load the meta data from.
     */
    void loadMeta (const std::string & name);

    /**
     * Save the model to the given file.
     * @param name file to save the model to.
     */
    void saveModel (const std::string & name);
    
    /**
     * Save the model meta data to the given file.
     * @param name file to save the meta data to.
     */
    void saveMeta (const std::string & name) const;

    /**
     * Add the given sprite to the model.
     * @param name filename of the sprite.
     */
    void addSprite (const std::string & name);

    /**
     * Remove a sprite from the model.
     */
    void removeSprite ();

    /**
     * Add a new model to the UI.
     * @param model the actual model.
     */
    void addModel (world::placeable_model *model);    
    
    /**
     * Add a new shape to the selected model.
     */
    void addShape ();

    /**
     * Copy shapes from currently selected model.
     */
    void copyShapes ();

    /**
     * Paste previously copied shape into the selected model.
     */
    void pasteShapes ();
    
    /**
     * Remove a shape from the selected model.
     */
    void removeShape ();
    
    /**
     * Update the current shape's solid flag.
     * @param is_solid whether to make the shape solid or not.
     */
    void setSolid (const bool & is_solid);

    /**
     * Update shape list from selected model.
     * @param model the model whose shapes to display.
     */
    void updateShapeList (world::placeable_model *model);
    
    /**
     * Check whether there is a shape to paste into sprite.
     * @return true if shape is available for pasting, false otherwise.
     */
    bool shapeInClipboard () const
    {
        return ShapeToPaste != NULL;
    }
    
    /**
     * Add a new tag to the model.
     * @param tag the tag to add.
     */
    void addTag (gchar *tag = NULL);

    /**
     * Remove a tag from the model.
     */
    void removeTag ();

    /**
     * Add a new connector to the model.
     * @param ctor the connector to add.
     */
    void addConnector (MdlConnector *ctor = NULL);

    /**
     * Remove a connector from the model.
     */
    void removeConnector ();

    /**
     * Change the side the connector is attached to.
     * @param connector the model connector
     * @param side the new side the connector is attached to.
     */
    void updateConnectorFace (MdlConnector *connector, MdlConnector::face side);

    /**
     * Change the position of the connector.
     * @param connector the model connector.
     * @param pos the new position of the connector.
     */
    void updateConnectorPos (MdlConnector *connector, const s_int16 & pos);

    /**
     * Enable or disable the widget with the given id.
     * @param id the widget name
     * @param sensitive true to enable, false to disable widget
     */
    void setActive (const std::string & id, const bool & sensitive);
    
    /**
     * Toggle the editing mode.
     * @param editing_mode a recognized editing mode.
     */
    void setEditingMode (const int & editing_mode);

    /**
     * Update zoom level of displayed model.
     */
    void zoom ();

protected:
    /**
     * Update main window title.
     * @param modified whether the edited model is modified.
     */
    void setTitle(const bool & modified);

private:
    /// a shape that has been copied
    world::placeable_shape *ShapeToPaste;
    /// the main window
    GtkWidget *Window;
    /// the user interface
    GtkBuilder *Ui;
    /// the editing area
    GuiPreview *Preview;
    /// directory of last loaded sprite
    std::string SpriteDir;
    /// directory of last model
    std::string ModelDir;
    /// name of model
    std::string Filename;
    /// name of first sprite added
    std::string Spritename;
};

#endif
