/*
   $Id: gui_mapedit.h,v 1.2 2009/04/03 22:00:46 ksterker Exp $

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
 * @file mapedit/gui_mapedit.h
 *
 * @author Kai Sterker
 * @brief The Map Editor's main window
 */

#ifndef GUI_MAPEDIT_H
#define GUI_MAPEDIT_H

#include <string>
#include <vector>

class GuiMapview;
class GuiEntityList;
class MapData;

/**
 * The application's main window. Contains the menu- and statusbar, as
 * well as pointers to the other widgets. The main window is created at
 * program startup, and closed on termination of the program. There can
 * be only one main window at a time, but several maps may be open
 * simultanously.
 */
class GuiMapedit
{
public:
    /** 
     * Constructor. Creates the main window with all the sub-widgets,
     * such as the map view or the object library.
     */
    GuiMapedit ();
    /**
     * Destructor.
     */
    ~GuiMapedit ();
    
    /**
     * Global pointer to the main window, to allow easy access from anywhere.
     */
    static GuiMapedit *window;

    /**
     * Widget access methods
     */
    //@{
    /**
     * Retrieve the map view.
     * @return the widget containing the map being edited.
     */
    GuiMapview *view () const     { return View; }
    /**
     * Retrieve the main window of the applictaion.
     * @return a GtkWidget.
     */
    GtkWidget *getWindow () const { return Wnd; }
    //@}
    
    /**
     * Load and display given map.
     * @param fname name of the map to load.
     */
    void loadMap (const std::string & fname);

    /**
     * Save active map.
     * @param fname name under which to save the map.
     */
    void saveMap (const std::string & fname);
    
    /**
     * Display location (of cursor) in the status bar.
     * @param x x-coordinate.
     * @param y y-coordinate.
     * @param z z-coordinate.
     */
    void setLocation (const int & x, const int & y, const int & z);
    
    /**
     * Get the directory where the last fileselection took place.
     * @return Path to the directory last opened.
     */
    std::string directory () const { return Directory + "/"; }
    /**
     * Get the full path and name of the map currently in the view.
     * @return Filename of the current map.
     */
    std::string filename () const;
                        
private:
    /**
     * (Re)build the 'windows' menu 
     */
    void initMenu ();
    /**
     * (Re)build the 'open recent' sub-menu.                      
     */
    void initRecentFiles ();
    /**
     * Display the correct window title
     */
    void initTitle ();
    /**
     * Set the GUI back to it's initial state.
     */
    void clear ();

    /// entity list
    GuiEntityList *EntityList;
    /// map view
    GuiMapview *View;                 
    /// actual main window
    GtkWidget *Wnd;
    /// directory used in last file-selection
    std::string Directory;  
    
    /// a status bar
    GtkWidget *StatusCoordinates;
    
    /// list of loaded maps
    std::vector<MapData*> LoadedMaps;
    /// index of currently active map
    int ActiveMap;
};

#endif // GUI_MAPEDIT_H
