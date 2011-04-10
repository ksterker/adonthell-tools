/*
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
 * @file modeller/gui_preview.h
 *
 * @author Kai Sterker
 * @brief Where the model is displayed and assembled
 */


#ifndef GUI_PREVIEW_H
#define GUI_PREVIEW_H

#include "mdl_renderer.h"

/**
 * This shows the model as it is assembled from sprites and shapes.
 * Allows to position the sprites and shapes relative to each other
 * and to manipulate the size and shape of shapes.
 */
class GuiPreview 
{
public:
    /**
     * Create new preview.
     * @param drawing_area the surface to draw on
     * @param shape_data gui elements for displaying shape information.
     */
    GuiPreview (GtkWidget *drawing_area, GtkEntry** shape_data, GtkTreeModel *models);

    /**
     * Update (part of) the screen from backing pixmap.
     * @param sx x coordinate
     * @param sy y coordinate
     * @param l length
     * @param h height
     */
    void draw (const int & sx, const int & sy, const int & l, const int & h);

    /**
     * Update size of the preview.
     * @param widget widget containing the new size.
     */
    void resizeSurface (GtkWidget *widget);

    /**
     * Update contents of the backing pixmap.
     */
    void render ();
    
    /**
     * Render the given part of the preview and update screen.
     * @param sx starting x offset
     * @param sy starting y offset
     * @param l length of the area to render
     * @param h height of the area to render
     */
    void render (const int & sx, const int & sy, const int & l, const int & h);
    
    /**
     * Process mouse movement over preview area.
     * @param point location of the mouse pointer.
     */
    void mouseMoved (const GdkPoint *point);

    /**
     * Modifying selected shape.
     */
    //@{
    /**
     * Set initial drag position when selecting a shape handle.
     * @param point location of the mouse pointer.
     */
    void startDragging (GdkPoint *point);

    /**
     * Process dragging shape handles.
     * @param point location of the mouse pointer.
     * @param modifier whether modifier (shift) key is pressed.
     */
    void handleDragged (GdkPoint *point, const bool & modifier);

    /**
     * Return whether a handle is currently being dragged.
     * @return true if this is the case, false otherwise.
     */
    bool isHandleDragged () const 
    { 
        return PrevPos != NULL; 
    }

    /**
     * Stop dragging the current handle.
     */
    void stopDragging ()
    {
        delete PrevPos;
        PrevPos = NULL;
    }
    //@}

    /**
     * Update shape from value entered into the field.
     * @param entry a text entry field.
     */
    void updateShape (GtkEntry *entry);

    /**
     * Set the model being displayed/edited.
     * @param model the model being edited.
     */
    void setCurModel (world::placeable_model *model);
    
    /**
     * Set the shape being displayed/edited.
     * @param shape the shape being edited.
     */
    void setCurShape (world::cube3 *shape);

protected:
    /**
     * Indicate which value will be affected by dragging a handle.
     * @param handle index of the handle being dragged.
     * @param highlight true to turn indicator on, false to turn indicator off.
     */
    void indicateEditingField (const s_int32 & handle, const bool & highlight);

    /**
     * Display size and position of current shape.
     */
    void updateShapeData () const;

    /**
     * Fill in one piece of data of the given shape.
     * @param data the index into the ShapeData array.
     * @param value the new value to display.
     */
    void setShapeData (const u_int32 & data, const s_int32 & value) const;

private:
    /// the renderer
    ModelRenderer Renderer;
    /// the display widget
    GtkWidget *DrawingArea;
    /// the fields for displaying/editing shape data
    GtkEntry **ShapeData; 
    /// list of model parts
    GtkTreeModel *ModelList;
    /// the render surface
    gfx::surface *Target;
    /// for rendering UI elements
    gfx::surface *Overlay;
    /// the shape currently being edited 
    world::cube3 *Shape;
    /// the model currently being edited
    world::placeable_model *Model;
    /// the currently selected handle
    int SelectedHandle;
    /// previous position when dragging
    GdkPoint *PrevPos;
    /// the position of the edit handles
    GdkPoint Handles[4];
};

#endif
