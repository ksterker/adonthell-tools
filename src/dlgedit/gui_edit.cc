/*
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
 * @file gui_edit.cc
 *
 * @author Kai Sterker
 * @brief Wrapper around the gtkeditor widget.
 */

#include <pango/pango-font.h>
#include "gui_edit.h"

// ctor
GuiEdit::GuiEdit (GtkWidget *container)
{
    // let the editor be scrollable
    GtkWidget *scrolled = gtk_scrolled_window_new (0, 0);
    gtk_container_add (GTK_CONTAINER (container), scrolled);
    gtk_container_set_border_width (GTK_CONTAINER (scrolled), 4);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);
    gtk_widget_show (scrolled);

    // create the editor
    entry = gtk_text_buffer_new (NULL);
    view = gtk_text_view_new_with_buffer (entry);
    gtk_container_add (GTK_CONTAINER (scrolled), view);
    gtk_widget_set_can_default(view, TRUE);
    gtk_text_view_set_editable (GTK_TEXT_VIEW(view), TRUE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW(view), GTK_WRAP_WORD);
    gtk_widget_show (view);

    // use a fixed size font
    PangoFontDescription *font_desc = pango_font_description_from_string ("Fixed Monospace 12");
    gtk_widget_modify_font (view, font_desc);
    pango_font_description_free (font_desc);
}

// dtor
GuiEdit::~GuiEdit ()
{
}

// set the entry's text
void GuiEdit::setText (const std::string &text)
{
    gtk_text_buffer_set_text (entry, text.c_str (), -1);
}

// return the entry's text
std::string GuiEdit::getText ()
{
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds (entry, &start, &end);
    
    gchar *tmp = gtk_text_buffer_get_text (entry, &start, &end, TRUE);
    std::string text (tmp);
    g_free (tmp);

    return text;
}
