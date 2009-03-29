/*
 $Id: gui_mapview.h,v 1.1 2009/03/29 12:27:26 ksterker Exp $
 
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

class MapData;

/**
 * The widget handling the graphical representation of the map.
 * Displays a map view and offers an interface for authors to edit
 * the map. Only one instance of this class exists throughout the 
 * whole mapedit session, but the map it displays can be changed.
 */
class GuiMapview
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
    ~GuiMapview ();

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
     * Render the map to screen.
     */
    void draw ();
    
    /**
     * Change view size after user modified size of application window.
     * @param widget widget containing the new valid size.
     */
    void resizeSurface (GtkWidget *widget);

private:
    /// Drawing Area
    GtkWidget *Screen;
    /// The actual mapview
    world::mapview *View;
};

#endif // GUI_MAPVIEW_H
