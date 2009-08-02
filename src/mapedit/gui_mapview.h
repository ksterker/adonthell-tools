/*
 $Id: gui_mapview.h,v 1.7 2009/05/24 13:40:28 ksterker Exp $
 
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
 * @file mapedit/gui_mapview.h
 *
 * @author Kai Sterker
 * @brief View for the map.
 */

#ifndef GUI_MAPVIEW_H
#define GUI_MAPVIEW_H

#include <world/mapview.h>

#include "gui_scrollable.h"
#include "map_renderer.h"

namespace gfx
{
    class surface;
};

class MapEntity;
class MapData;
class GuiGrid;

/**
 * The widget handling the graphical representation of the map.
 * Displays a map view and offers an interface for authors to edit
 * the map. Only one instance of this class exists throughout the 
 * whole mapedit session, but the map it displays can be changed.
 */
class GuiMapview : public Scrollable
{
public:
    /** 
     * Constructor. Main purpose is to create the drawing area for the
     * map and to register the GTK+ event callbacks used to catch user 
     * input for this widget.
     *
     * @param paned The GtkPaned that will hold the map view widget.
     */
    GuiMapview (GtkWidget* paned);
    
    /**
     * Standard desctructor.
     */
    virtual ~GuiMapview ();

    /**
     * Reset to initial state.
     */
    void clear () {};
        
    /**
     * Set the map to display and edit.
     * @param area the map.
     */
    void setMap (MapData *area);
    
    /**
     * @name Drawing
     */
    //@{
    /**
     * Draw the given area to screen.
     * @param sx starting x offset
     * @param sy starting y offset
     * @param l length of the area to render
     * @param h height of the area to render
     */
    void draw (const int & sx, const int & sy, const int & l, const int & h);

    /**
     * Render the mapview to screen.
     */
    void render ();
        
    /**
     * Render the given part of the mapview and update screen.
     * @param sx starting x offset
     * @param sy starting y offset
     * @param l length of the area to render
     * @param h height of the area to render
     */
    void render (const int & sx, const int & sy, const int & l, const int & h);
    
    /**
     * Render the given object.
     * @param obj the object to render.
     */
    void renderObject (world::chunk_info *obj);
    //@}
    
    /**
     * Change view size after user modified size of application window.
     * @param widget widget containing the new valid size.
     */
    void resizeSurface (GtkWidget *widget);

    /**
     * Notification of mouse movement.
     * @param point location of mouse pointer.
     */
    void mouseMoved (const GdkPoint * point);
    
    /**
     * Update height of mapview (and objects being placed)
     * by the given offset.
     * @param oz offset for z axis.
     */
    void updateHeight (const s_int16 & oz);
    
    /**
     * @name Editing Functionality
     */
    //@{
    /**
     * Pick the currently highlighted object for map editing.
     */
    void selectCurObj ();
    /**
     * Pick the given object for map editing, discarding
     * any previously picked object.
     * @param ety the new object to use for map editing.
     */
    void selectObj (MapEntity *ety);
    /**
     * Add currently selected object to the map.
     */
    void placeCurObj ();
    /**
     * Get the object that is currently used for "painting"
     * the map, or NULL if there is no such object.
     * @return object used for map editing.
     */
    MapEntity *getSelectedObject() const { return DrawObj; }
    /**
     * Discard the object that is currently used for map editing.
     */
    void releaseObject ();
    /**
     * Erase the currently highlighted object from the map.
     */
    void deleteCurObj ();
    //@}
    
    /**
     * @name Auto-Scrolling (TM) ;) functionality.
     */
    //@{
    /**
     * Check whether it is allowed to initiate scrolling.
     * @return false if scrolling is forbidden, true otherwise.
     */
    virtual bool scrollingAllowed () const;
    /**
     * Moves the view in the desired direction.
     */
    virtual void scroll ();
    /**
     * Return the widget being scrolled.
     * @return the widget being scrolled.
     */
    virtual GtkWidget* drawingArea() const { return Screen; }
    //@}
    
protected:
    /**
     * Get the size and position of the (rendered) object in pixels.
     * @param obj the object whose size to calculate.
     * @param x will receive the x-coordinate
     * @param y will receive the y-coordinate
     * @param l will receive the extend in x direction
     * @param h will receive the extend in y direction
     */
    void getObjectExtend (world::chunk_info *obj, int & x, int & y, int & l, int & h);
    /**
     * Will highlight the object below the cursor, if any.
     */
    void highlightObject ();
    /**
     * Show whether the object we paint with is overlapping objects
     * on the map at the current position.
     */
    void indicateOverlap ();

private:
    /// Drawing Area
    GtkWidget *Screen;
    /// The actual mapview
    world::mapview *View;
    /// The renderer
    MapRenderer Renderer;
    /// The render target for the map view
    gfx::surface *Target;
    /// Overlay for additional visuals
    gfx::surface *Overlay;

    /// The grid to which mapobjects can be aligned
    GuiGrid *Grid;
    
    /// The currently highlighted object
    MapEntity *CurObj;
    /// The object used for "drawing"
    MapEntity *DrawObj;
    
    /// A rendered version of the object used for "drawing"
    gfx::surface *DrawObjSurface;
    /// Position of object used for drawing
    world::vector3<s_int32> DrawObjPos;
};

#endif // GUI_MAPVIEW_H
