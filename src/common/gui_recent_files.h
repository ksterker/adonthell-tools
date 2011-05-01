/*
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file common/gui_recent_files.h
 *
 * @author Kai Sterker
 * @brief Keeps a list of recent files.
 */

#ifndef GUI_RECENT_FILES_H
#define GUI_RECENT_FILES_H

#include <string>
#include <gtk/gtk.h>

/**
 * Interface for anyone interested in recent file actions.
 */
class GuiRecentFileListener
{
public:
    virtual void OnRecentFileActivated (const std::string & file) = 0;
};

/**
 * Keeps a list of recently opened file for the application.
 * Also adds the file to the desktop-wide recent file list
 * and allows to launch the program with that file.
 */
class GuiRecentFiles
{
public:
    /**
     * Create a recent files list for the given application
     * @param cmd the executable
     * @param args the required command line arguments
     */
    GuiRecentFiles (const std::string & cmd, const std::string & args);

    /**
     * Destructor.
     */
    virtual ~GuiRecentFiles();

    /**
     * Register the given file with given mimetype as a recently opened file.
     * @param file the absolute pathname to the file.
     * @param type the files mimetype.
     */
    void registerFile (const std::string & file, const std::string & type);

    /**
     * Register a listener to get notified when a recent file
     * has been selected.
     * @param listener the new listener to set
     */
    void setListener (GuiRecentFileListener *listener)
    {
        Listener = listener;
    }

    /**
     * Get the listener that wants to be notified when a recent
     * file has been selected.
     * @return the current listener
     */
    GuiRecentFileListener *getListener () const
    {
        return Listener;
    }

    /**
     * Return a menu with the list of recent files.
     */
    GtkWidget *recentFileMenu () const
    {
        return Menu;
    }

private:
    /// the application to launch
    std::string Command;
    /// the command line arguments
    std::string Arguments;
    /// the recent files manager
    GtkRecentManager* Manager;
    /// the menu displaying recently used files
    GtkWidget *Menu;
    /// listener to notify when a recent file was selected
    GuiRecentFileListener *Listener;
};

#endif /* GUI_RECENT_FILES_H_ */
