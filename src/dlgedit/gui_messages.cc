/*
   $Id: gui_messages.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

   Copyright (C) 2002 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file gui_messages.cc
 *
 * @author Kai Sterker
 * @brief Defines the messages to display in the status bar.
 */

#include <string>
#include <gtk/gtk.h>
#include "gui_messages.h"

// define all the different messages
GuiMessages::GuiMessages (GtkWidget *sb)
{
    current = 0;
    statusbar = sb;
    
    // Menu Help Messages
    messages[1]     = " Create new dialogue";
    messages[2]     = " Load dialogue from disk";
    messages[3]     = " Save dialogue to disk";
    messages[4]     = " Save dialogue under new name";
    messages[5]     = " Close current dialogue";
    messages[6]     = " Exit the program";
    messages[7]     = " Reload dialogue from disk, discarding recent changes.";
    
    messages[10]    = " Change the preferences of the current dialogue";
    messages[11]    = " Customize imports, constructor, destructor and methods of the dialogue script";
    messages[12]    = " Transform the current dialogue into a Python script, as required by the dialogue engine";
    messages[13]    = " Preview a translation of the current dialogue";
    messages[14]    = " Start the dialogue engine with the current dialogue";
    
    messages[20]    = " Switch the view to this dialogue";
    
    // Dialogue Editing Help Messages
    messages[100]   = " LEFT-click to select Circle or MIDDLE-click to edit it";
    messages[101]   = " LEFT-click to select Arrow";
    messages[102]   = " LEFT-click to create new Circle";
    
    messages[110]   = " RIGHT-click to cancel selection or press DEL to delete selected node";
    messages[111]   = " LEFT-click to connect the two Circles";
    messages[112]   = " LEFT-click to create new Circle, connected with the selected one";
    messages[113]   = " MIDDLE-click/press ENTER to edit selected Circle";
    
    messages[120]   = " Release LEFT mouse button to attach Arrow to this Circle";
    
    messages[130]   = " All editing functions are disabled.  Press CTRL+P to leave preview mode";
            
    // Report Success
    messages[200]   = " Loading successful";
    messages[201]   = " Saving successful";
    messages[202]   = " Sub-dialogue loaded successfully";
    
    messages[210]   = " Settings updated";
    messages[211]   = " Custom code updated";
    messages[212]   = " Dialogue compiled successfully";
    
    // Welcome Message
    messages[1000]  = " Welcome to the Adonthell Dialogue Editor";
    
    // Report Failure
    messages[-2]    = " Could not open '%s'. Please make sure the file exists and is readable";
    messages[-3]    = " Could not load '%s'. Either this is no dialogue source, or it has been corrupted";
    messages[-4]    = " Saving of '%s' failed.";

    messages[-100]  = " Cannot connect two PLAYER Circles";
    
    // Preview Translations
    messages[-130]  = " '%s' does not appear to be a binary catalog file";
    messages[-131]  = " Failed to create temporary locale dir";
}

// display a message to the statusbar
void GuiMessages::display (int id)
{
    // no change needed
    if (id == current) return;
    
    const char *message = messages[id];

    // some context id the statusbar needs for some reason
    id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "Message");
    
    // remove the previous message
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), id);
    
    // add the new status
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), id, message);
}

// display a message with some custom text inserted
void GuiMessages::display (int id, const char* text)
{
    if (id == current) return;
    
    std::string message (messages[id]);
    unsigned int pos = message.find ("%s");
    
    // insert text into the message
    if (pos != message.npos)
        message.replace (pos, 2, text);
            
    // some context id the statusbar needs for some reason
    id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "Message");
    
    // remove the previous message
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), id);
    
    // add the new status
    gtk_statusbar_push (GTK_STATUSBAR (statusbar), id, message.c_str ());
}

// clear the statusbar
void GuiMessages::clear ()
{
    // some context id the statusbar needs for some reason
    int id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "Message");
    
    // remove the previous message
    gtk_statusbar_pop (GTK_STATUSBAR (statusbar), id);    
}
