/*
   $Id: gui_dlgedit.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

   Copyright (C) 2002/2003 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Dlgedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dlgedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file gui_dlgedit.h
 *
 * @author Kai Sterker
 * @brief The Dialogue Editor's main window
 */

#ifndef GUI_DLGEDIT_H
#define GUI_DLGEDIT_H

#include "gui_messages.h"
#include "gui_graph.h"
#include "gui_list.h"
#include "gui_tree.h"

/**
 * The application's main window. Contains the menu- and statusbar, as
 * well as pointers to the other widgets. The main window is created at
 * program startup, and closed on termination of the program. There can
 * be only one main window at a time, but several dialogues may be open
 * simultanously.
 */
class GuiDlgedit
{
public:
    /** 
     * Constructor. Creates the main window with all the sub-widgets,
     * such as the dialogue view or the instant preview.
     */
    GuiDlgedit ();
    /**
     * Destructor.
     */
    ~GuiDlgedit ();
    
    /**
     * Global pointer to the main window, to allow easy access from anywhere.
     */
    static GuiDlgedit *window;

    /**
     * Widget access methods
     */
    //@{
    /**
     * Retrieve the dialogue view.
     * @return the widget containing the graphical representation of the
     *         dialogue graph.
     */
    GuiGraph *graph ()      { return graph_; }
    /**
     * Retrieve the instant preview widget.
     * @return the widget containing the text of the selected DlgNode and
     *         that of it's parents and children.  
     */
    GuiList *list ()        { return list_; }
    /**
     * Retrieve the module structure view.
     * @return the widget displaying the dialogue and its sub-modules.
     */
    GuiTree *tree ()        { return tree_; }
    
    /**
     * Retrieve the main window of the applictaion.
     * @return a GtkWidget.
     */
    GtkWidget *getWindow () { return wnd; }
    //@}
    
    /**
     * Set the program state and display it in the status bar
     * @param mode The new program state.
     */
    void setMode (mode_type mode);
    /**
     * Get the current program state
     * @return The program state.
     */
    mode_type mode ()   { return mode_; }
    
    /**
     * Create a new dialogue.
     */
    void newDialogue ();
    /**
     * Load a (top level) dialogue from a file
     * @param file Filename (and path) of the dialogue to load.
     */
    void loadDialogue (const std::string &file);
    /**
     * Load a sub-dialogue from a file. Sub-dialogues are not
     * directly available for editing; instead they become part
     * of a (top level) dialogue.
     * @param file Filename (and path) of the dialogue to load.
     * @return the sub-dialogue, or \b NULL if loading failed.
     */
    DlgModule* loadSubdialogue (const std::string &file);
    /**
     * Save a dialogue to file
     * @param file Filename (and path) of the dialogue to load.
     */
    void saveDialogue (const std::string &file);
    /**
     * Reload dialogue from disk if possible. This discards all
     * changes since the last save.
     */
    void revertDialogue ();
    /**
     * Close a dialogue
     */
    void closeDialogue ();
    /**
     * Display a dialogue
     * @param module The dialogue to display.
     * @param center whether to center the dialgue in view or not
     */
    void showDialogue (DlgModule *module, bool center = false);
    /**
     * Compile a dialogue
     */
    void compileDialogue ();
    /**
     * Test whether the given filename points to a valid dialogue
     * @param file path of the dialogue
     * @return <b>true</b> if the dialogue is valid, <b>false</b> otherwise.
     */
    static bool checkDialogue (const std::string &file);
    /**
     * Edit the custom code of the current module.
     */
    void customCode ();
    /**
     * Edit the settings of the current module.
     */
    void settings ();
    /**
     * preview translation for a dialogue
     * @param catalogue full path to a gnu gettext compliant binary catalogue
     *        (.mo) file
     */
    void previewTranslation (const std::string &catalogue);
    /**
     * stop translation preview
     */
    void exitPreview ();
    /**
     * Indicate that the active dialogue has been changed by the user
     */
    void setChanged (); 
    /**
     * Indicate that the acive dialogue has been assigned to a new project.
     */
    void updateProject ();
    /**
     * Get the directory where the last fileselection took place.
     * @return Path to the directory last opened.
     */
    std::string directory ()  { return directory_+"/"; }
    /**
     * Get the full path and name of the dialogue currently in the view.
     * @return Filename of the current dialogue.
     */
    std::string filename ();
                        
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
     * Add a dialogue to the GUI
     */
    DlgModule *initDialogue (std::string);
    /**
     * Set the GUI back to it's initial state.
     */
    void clear ();

    int number;                     // serial number of open dialogues
    mode_type mode_;                // the program mode
    GuiList *list_;                 // instant preview widget
    GuiTree *tree_;                 // dialogue structure view
    GuiGraph *graph_;               // dialogue view
    GuiMessages *message;           // statusbar for displaying help/error texts
    GtkWidget *wnd;                 // actual main window
    GtkWidget *menuItem[MAX_ITEM];  // pointers to a few menu-items
    GtkWidget *windowMenu;          // the 'Windows' dropdown menu
    GtkWidget *status_mode;         // statusbar displaying the program state
    std::vector<DlgModule*> dialogues_;// dialogues currently loaded
    std::string directory_;            // directory used in last file-selection
    static char *progState[NUM_MODES]; // Textual program states
};

#endif // GUI_DLGEDIT_H
