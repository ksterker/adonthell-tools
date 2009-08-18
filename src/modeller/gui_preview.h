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
 * @file modeller/gui_preview.h
 *
 * @author Kai Sterker
 * @brief Where the model is displayed and assembled
 */


#ifndef GUI_PREVIEW_H
#define GUI_PREVIEW_H

namespace gfx
{
    class surface;
};

namespace world
{
    class placeable_model;
};

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
     */
    GuiPreview (GtkWidget *drawing_area);

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
     * Set the model being displayed/edited.
     * @param model the model being edited.
     */
    void setCurModel (world::placeable_model *model);
    
private:
    /// the display widget
    GtkWidget *DrawingArea;
    /// the render surface
    gfx::surface *Target;
    /// the model currently being edited
    world::placeable_model *Model;
};

#endif
