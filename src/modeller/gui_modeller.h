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

namespace world
{
    class placeable_model;
};

class GuiPreview;

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
     * Start working on a new model. Clears all current data.
     */
    void newModel ();

    /**
     * Load the model from the given file.
     * @param name file to load the model from.
     */
    void loadModel (const std::string & name);    
    
    /**
     * Save the model to the given file.
     * @param name file to save the model to.
     */
    void saveModel (const std::string & name);
    
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
     * Remove a shape from the selected model.
     */
    void removeShape ();
    
    /**
     * Update shape list from selected model.
     * @param model the model whose shapes to display.
     */
    void updateShapeList (world::placeable_model *model);
    
    /**
     * Enable or disable the widget with the given id.
     * @param id the widget name
     * @param sensitive true to enable, false to disable widget
     */
    void setActive (const std::string & id, const bool & sensitive);
    
private:
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
};

#endif
