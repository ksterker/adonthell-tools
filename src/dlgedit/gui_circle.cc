/*
   $Id: gui_circle.cc,v 1.4 2009/03/29 19:35:00 ksterker Exp $ 

   Copyright (C) 2002/2004/2007 Kai Sterker <kaisterker@linuxgames.com>
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

#include <string.h>
#include <gtk/gtk.h>
#include "gui_circle.h"
#include "gui_circle_events.h"

/**
 * @file gui_circle.cc
 *
 * @author Kai Sterker
 * @brief The Edit Circle window
 */

GuiCircle::GuiCircle (GtkWindow *parent, node_type *t, DlgCircleEntry *e, DlgModuleEntry *dme) : GuiModalDialog (parent)
{
    entry = e;
    type = t;

    GtkWidget *vbox1;
    GtkWidget *notebook1;
    GtkWidget *vbox2;
    GtkWidget *scrolledwindow;
    GtkWidget *label;
    GSList *character_group = NULL;
    GtkWidget *player_button;
    GtkWidget *narrator_button;
    GtkWidget *hbox1;
    GtkWidget *npc_button;
    GtkWidget *npc_selection_menu;
    GtkWidget *glade_menuitem;
    GtkWidget *npc_mood;
    GtkWidget *npc_mood_menu;
    GtkWidget *frame1;
    GtkWidget *hbuttonbox1;
    GtkWidget *ok_button;
    GtkWidget *cancel_button;
    GtkWidget *ebox;
    GtkWidget *text_view;
    GtkWidget *annotation_view;
    
    // color to use for Player and Narrator text and labels    
    GdkColor dark_blue  = { 0, 0, 0, 35000 };
    GdkColor green      = { 0, 0, 27300, 15600 };
    GdkColor dark_green = { 0, 0, 27500, 0 };

    // Tooltips for the window
    GtkTooltips *tooltips = gtk_tooltips_new ();

    GtkStyle *style = NULL;
     
    // the "Edit Dialogue Node" window
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_object_set_data (GTK_OBJECT (window), "window", window);
    gtk_widget_set_usize (window, 400, 320);
    gtk_window_set_title (GTK_WINDOW (window), "Edit Dialogue Node");
    gtk_window_set_position (GTK_WINDOW (window), GTK_WIN_POS_MOUSE);
    gtk_window_set_modal (GTK_WINDOW (window), TRUE);
    gtk_window_set_resizable (GTK_WINDOW (window), FALSE);
    
    vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox1);
    gtk_object_set_data_full (GTK_OBJECT (window), "vbox1", vbox1, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox1);
    gtk_container_add (GTK_CONTAINER (window), vbox1);

    // the GtkNotebook with the different 'pages' of the window
    notebook1 = gtk_notebook_new ();
    gtk_widget_ref (notebook1);
    gtk_object_set_data_full (GTK_OBJECT (window), "notebook1", notebook1, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (notebook1);
    gtk_box_pack_start (GTK_BOX (vbox1), notebook1, TRUE, TRUE, 0);
    gtk_notebook_set_show_border (GTK_NOTEBOOK (notebook1), FALSE);
    gtk_notebook_set_tab_hborder (GTK_NOTEBOOK (notebook1), 6);
    GTK_WIDGET_UNSET_FLAGS (notebook1, GTK_CAN_FOCUS);

    // Page 1: Edit Text
    vbox2 = gtk_vbox_new (FALSE, 0);
    gtk_widget_ref (vbox2);
    gtk_object_set_data_full (GTK_OBJECT (window), "vbox2", vbox2, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (vbox2);
    gtk_container_add (GTK_CONTAINER (notebook1), vbox2);
    gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);

    // Scrollbar for the text entry
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow);
    gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow1", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow);
    gtk_box_pack_start (GTK_BOX (vbox2), scrolledwindow, TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    // the text entry
    text_entry = gtk_text_buffer_new (NULL);
    text_view = gtk_text_view_new_with_buffer (GTK_TEXT_BUFFER (text_entry));
    gtk_widget_ref (text_view);
    gtk_object_set_data_full (GTK_OBJECT (window), "text_view", text_view, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (text_view);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), text_view);
    GTK_WIDGET_SET_FLAGS (text_view, GTK_CAN_DEFAULT);
    gtk_tooltips_set_tip (tooltips, text_view, "Enter the Text to appear within the conversation", NULL);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (text_view), TRUE);
    gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (text_view), GTK_WRAP_WORD);
    
    // set the text of the entry
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (text_entry), e->text ().c_str (), -1);
      
    // The radiobuttons to chose the text-type from
    label = gtk_label_new ("Narrator");
    gtk_label_set_justify ((GtkLabel *) label, GTK_JUSTIFY_LEFT);
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

    // make the narrator-radiobutton-text green
    style = gtk_style_copy (gtk_widget_get_default_style ());
    style->fg[GTK_STATE_NORMAL] = dark_green;
    style->fg[GTK_STATE_ACTIVE] = dark_green;
    style->fg[GTK_STATE_PRELIGHT] = dark_green;
    style->fg[GTK_STATE_SELECTED] = dark_green;
    gtk_widget_set_style (label, style);
    gtk_widget_show (label);

    narrator_button = gtk_radio_button_new (character_group);
    gtk_container_add (GTK_CONTAINER(narrator_button), label);
    character_group = gtk_radio_button_group (GTK_RADIO_BUTTON (narrator_button));
    gtk_widget_ref (narrator_button);
    gtk_object_set_data_full (GTK_OBJECT (window), "narrator_button", narrator_button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_object_set_data (GTK_OBJECT (narrator_button), "type", GINT_TO_POINTER (NARRATOR));
    gtk_widget_show (narrator_button);
    gtk_box_pack_start (GTK_BOX (vbox2), narrator_button, FALSE, FALSE, 0);
    GTK_WIDGET_UNSET_FLAGS (narrator_button, GTK_CAN_FOCUS);

    // activate the narrator button if neccessary
    if (*type == NARRATOR)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (narrator_button), TRUE);
        on_radio_button_pressed (GTK_BUTTON (narrator_button), text_view);
    }
    // the player button
    label = gtk_label_new ("Player");
    gtk_label_set_justify ((GtkLabel *) label, GTK_JUSTIFY_LEFT);
    gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);

    // make the player-radiobutton-text blue
    style = gtk_style_copy (gtk_widget_get_default_style ());
    style->fg[GTK_STATE_NORMAL] = dark_blue;
    style->fg[GTK_STATE_ACTIVE] = dark_blue;
    style->fg[GTK_STATE_PRELIGHT] = dark_blue;
    style->fg[GTK_STATE_SELECTED] = dark_blue;
    gtk_widget_set_style (label, style);
    gtk_widget_show (label);

    player_button = gtk_radio_button_new (character_group);
    gtk_container_add (GTK_CONTAINER(player_button), label);
    character_group = gtk_radio_button_group (GTK_RADIO_BUTTON (player_button));
    gtk_widget_ref (player_button);
    gtk_object_set_data_full (GTK_OBJECT (window), "player_button", player_button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_object_set_data (GTK_OBJECT (player_button), "type", GINT_TO_POINTER (PLAYER));
    gtk_widget_show (player_button);
    gtk_box_pack_start (GTK_BOX (vbox2), player_button, FALSE, FALSE, 0);
    GTK_WIDGET_UNSET_FLAGS (player_button, GTK_CAN_FOCUS);

    // activate the player button if neccessary
    if (*type == PLAYER)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (player_button), TRUE);
        on_radio_button_pressed (GTK_BUTTON (player_button), text_view);
    }

    hbox1 = gtk_hbox_new (FALSE, 10);
    gtk_widget_ref (hbox1);
    gtk_object_set_data_full (GTK_OBJECT (window), "hbox1", hbox1, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbox1);
    gtk_box_pack_start (GTK_BOX (vbox2), hbox1, FALSE, TRUE, 0);

    // the NPC radiobutton
    npc_button = gtk_radio_button_new_with_label (character_group, "NPC:");
    character_group = gtk_radio_button_group (GTK_RADIO_BUTTON (npc_button));
    gtk_widget_ref (npc_button);
    gtk_object_set_data_full (GTK_OBJECT (window), "npc_button", npc_button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_object_set_data (GTK_OBJECT (npc_button), "type", GINT_TO_POINTER (NPC));
    gtk_widget_show (npc_button);
    gtk_box_pack_start (GTK_BOX (hbox1), npc_button, FALSE, FALSE, 0);
    GTK_WIDGET_UNSET_FLAGS (npc_button, GTK_CAN_FOCUS);

    // activate the NPC button if neccessary
    if (*type == NPC)
    {
        gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (npc_button), TRUE);
        on_radio_button_pressed (GTK_BUTTON (npc_button), text_view);
    }

    // the option menu with the available NPC's
    npc_selection = gtk_option_menu_new ();
    gtk_widget_ref (npc_selection);
    gtk_object_set_data_full (GTK_OBJECT (window), "npc_selection", npc_selection, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (npc_selection);
    gtk_box_pack_start (GTK_BOX (hbox1), npc_selection, TRUE, TRUE, 0);
    gtk_tooltips_set_tip (tooltips, npc_selection, "Select the speaker", NULL);
    npc_selection_menu = gtk_menu_new ();
    glade_menuitem = gtk_menu_item_new_with_label ("Default");
    gtk_object_set_user_data (GTK_OBJECT (glade_menuitem), NULL);
    gtk_widget_show (glade_menuitem);
    gtk_menu_append (GTK_MENU (npc_selection_menu), glade_menuitem);
    gtk_option_menu_set_menu (GTK_OPTION_MENU (npc_selection), npc_selection_menu);
 
    for (std::string name = dme->character (); name != ""; name = dme->character ()) 
    {
        glade_menuitem = gtk_menu_item_new_with_label (name.c_str ());
        gtk_object_set_user_data (GTK_OBJECT (glade_menuitem), (void *) name.c_str ());
        gtk_widget_show (glade_menuitem);
        gtk_menu_append (GTK_MENU (npc_selection_menu), glade_menuitem);
        gtk_option_menu_set_menu (GTK_OPTION_MENU (npc_selection), npc_selection_menu);
    }

    // set the NPC this circle is assigned to    
    setOption (GTK_OPTION_MENU (npc_selection), e->npc ().c_str ());
    
    // the option menu with the available moods
    label = gtk_label_new ("Mood: ");
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_box_pack_start (GTK_BOX (hbox1), label, TRUE, FALSE, 0);
    gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_RIGHT);
    gtk_misc_set_alignment (GTK_MISC (label), 0.01, 0.5);

    npc_mood = gtk_option_menu_new ();
    gtk_widget_ref (npc_mood);
    gtk_object_set_data_full (GTK_OBJECT (window), "npc_mood", npc_mood, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (npc_mood);
    gtk_box_pack_start (GTK_BOX (hbox1), npc_mood, TRUE, TRUE, 0);
    gtk_tooltips_set_tip (tooltips, npc_mood, "How the speaker's image appears to the player", NULL);
    npc_mood_menu = gtk_menu_new ();
    glade_menuitem = gtk_menu_item_new_with_label ("Default");
    gtk_widget_show (glade_menuitem);
    gtk_menu_append (GTK_MENU (npc_mood_menu), glade_menuitem);
    gtk_option_menu_set_menu (GTK_OPTION_MENU (npc_mood), npc_mood_menu);

    // The heading of this page
    label = gtk_label_new ("Text");
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 0), label);

    // the entry for annotations
    frame1 = gtk_frame_new ("Annotations:");
    gtk_widget_ref (frame1);
    gtk_object_set_data_full (GTK_OBJECT (window), "frame1", frame1, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (frame1);
    gtk_box_pack_start (GTK_BOX (vbox2), frame1, TRUE, TRUE, 0);

    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow);
    gtk_object_set_data_full (GTK_OBJECT (window), "scrolledwindow4", scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow);
    gtk_container_add (GTK_CONTAINER (frame1), scrolledwindow);
    gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow), 4);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

    annotation_entry = gtk_text_buffer_new (NULL);
    annotation_view = gtk_text_view_new_with_buffer (annotation_entry);
    gtk_widget_ref (annotation_view);
    gtk_object_set_data_full (GTK_OBJECT (window), "annotation_view", annotation_view, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (annotation_view);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), annotation_view);
    gtk_tooltips_set_tip (tooltips, annotation_view, "Here is room for comments and other reminders", NULL);
    gtk_text_view_set_editable (GTK_TEXT_VIEW (annotation_view), TRUE);
    
    // insert text
    gtk_text_buffer_set_text (GTK_TEXT_BUFFER (annotation_entry), e->annotation ().c_str (), -1);
    
    // the loop checkbox
    ebox = gtk_event_box_new ();
    gtk_widget_ref (ebox);
    gtk_widget_show (ebox);
    gtk_object_set_data_full (GTK_OBJECT (window), "ebox", ebox, (GtkDestroyNotify) gtk_widget_unref);
    gtk_box_pack_end (GTK_BOX (vbox2), ebox, FALSE, FALSE, 0);
    gtk_tooltips_set_tip (tooltips, ebox, "Unless this is checked, the text becomes unavailable after being uttered.", NULL);
    
    loop = gtk_check_button_new_with_label ("Loop");
    gtk_widget_ref (loop);
    gtk_object_set_data_full (GTK_OBJECT (window), "loop", loop, (GtkDestroyNotify) gtk_widget_unref);
    gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (loop), e->loop ());
    gtk_widget_show (loop);
    gtk_container_add (GTK_CONTAINER (ebox), loop);

    // Condition-editor
    cond_edit = new GuiEdit (notebook1);

    // append tooltips
    gtk_tooltips_set_tip (tooltips, cond_edit->widget (),
        "Enter the conditions that have to be met before this node is available in the dialogue", NULL);

    // display the current conditios
    cond_edit->setText (entry->condition ());

    style = gtk_style_copy (gtk_widget_get_default_style ());
    style->fg[GTK_STATE_NORMAL] = green;
    style->fg[GTK_STATE_ACTIVE] = green;
    style->fg[GTK_STATE_PRELIGHT] = green;
    style->fg[GTK_STATE_SELECTED] = green;

    label = gtk_label_new ("Conditions");
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
    if (e->condition () != "") gtk_widget_set_style (label, style);
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 1), label);

    // Code-editor
    code_edit = new GuiEdit (notebook1);

    // append tooltips
    gtk_tooltips_set_tip (tooltips, code_edit->widget (),
        "Here you can enter arbitrary Python code to be executed when this line of dialogue is uttered.", NULL);
            
    // display the current code
    code_edit->setText (entry->code ());
    
    label = gtk_label_new ("Variables");
    gtk_widget_ref (label);
    gtk_object_set_data_full (GTK_OBJECT (window), "label", label, (GtkDestroyNotify) gtk_widget_unref);
    if (e->code () != "") gtk_widget_set_style (label, style);
    gtk_widget_show (label);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook1), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook1), 2), label);

    hbuttonbox1 = gtk_hbutton_box_new ();
    gtk_widget_ref (hbuttonbox1);
    gtk_object_set_data_full (GTK_OBJECT (window), "hbuttonbox1", hbuttonbox1, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (hbuttonbox1);
    gtk_box_pack_start (GTK_BOX (vbox1), hbuttonbox1, FALSE, TRUE, 0);
    gtk_button_box_set_layout (GTK_BUTTON_BOX (hbuttonbox1), GTK_BUTTONBOX_END);
    gtk_button_box_set_spacing (GTK_BUTTON_BOX (hbuttonbox1), 0);

    // ok button
    ok_button = gtk_button_new_with_label ("OK");
    gtk_widget_ref (ok_button);
    gtk_object_set_data_full (GTK_OBJECT (window), "ok_button", ok_button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (ok_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), ok_button);
    GTK_WIDGET_SET_FLAGS (ok_button, GTK_CAN_DEFAULT);

    // cancel button
    cancel_button = gtk_button_new_with_label ("Cancel");
    gtk_widget_ref (cancel_button);
    gtk_object_set_data_full (GTK_OBJECT (window), "cancel_button", cancel_button, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (cancel_button);
    gtk_container_add (GTK_CONTAINER (hbuttonbox1), cancel_button);
    GTK_WIDGET_SET_FLAGS (cancel_button, GTK_CAN_DEFAULT);

    // the various input handlers
    gtk_signal_connect (GTK_OBJECT (notebook1), "switch_page", GTK_SIGNAL_FUNC (on_switch_page), window);
    gtk_signal_connect (GTK_OBJECT (player_button), "clicked", GTK_SIGNAL_FUNC (on_radio_button_pressed), text_view);
    gtk_signal_connect (GTK_OBJECT (npc_button), "clicked", GTK_SIGNAL_FUNC (on_radio_button_pressed), text_view);
    gtk_signal_connect (GTK_OBJECT (narrator_button), "clicked", GTK_SIGNAL_FUNC (on_radio_button_pressed), text_view);
    gtk_signal_connect (GTK_OBJECT (ok_button), "clicked", GTK_SIGNAL_FUNC (on_circle_ok_button_pressed), this);
    gtk_signal_connect (GTK_OBJECT (cancel_button), "clicked", GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
    gtk_signal_connect (GTK_OBJECT (window), "delete_event", GTK_SIGNAL_FUNC (gtk_main_quit), NULL);
    gtk_signal_connect (GTK_OBJECT (window), "destroy", GTK_SIGNAL_FUNC (gtk_widget_destroyed), &window);
    
    // give focus to the text entry
    gtk_widget_grab_focus (text_view);
    gtk_widget_grab_default (text_view);
    gtk_object_set_data (GTK_OBJECT (window), "tooltips", tooltips);
}

// Apply the user's changes to the DlgCircle
void GuiCircle::applyChanges ()
{
    GtkTextIter start, end;
    gchar *text;

     // set the circle's type
    *type = (node_type) GPOINTER_TO_INT (g_object_get_data (G_OBJECT (text_entry), "type"));

    // get the contents of the different entries
    gtk_text_buffer_get_bounds (text_entry, &start, &end);
    text = gtk_text_buffer_get_text (text_entry, &start, &end, TRUE);
    entry->setText (text);
    g_free (text);

    gtk_text_buffer_get_bounds (annotation_entry, &start, &end);
    text = gtk_text_buffer_get_text (annotation_entry, &start, &end, TRUE);
    entry->setAnnotation (text);
    g_free (text);

    entry->setCode (code_edit->getText ());
    entry->setCondition (cond_edit->getText ());
    entry->setNpc (getOption (GTK_OPTION_MENU (npc_selection)));
    entry->setLoop (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (loop)));
}

// returns selected option
gchar *GuiCircle::getOption (GtkOptionMenu * o)
{
    GtkMenu *m = (GtkMenu *) gtk_option_menu_get_menu (o);
    GtkMenuItem *i = (GtkMenuItem *) gtk_menu_get_active (m);
    gchar *s = (gchar *) gtk_object_get_user_data (GTK_OBJECT (i));

    return s ? s : s = "Default";
}

// sets a default option
void GuiCircle::setOption (GtkOptionMenu *o, const gchar *label)
{
    GtkMenu *m = (GtkMenu *) gtk_option_menu_get_menu (o);
    GList *l = gtk_container_get_children (GTK_CONTAINER (m));
    gchar *c;
    int j = 0;

    while (l)
    {
        GtkMenuItem *i = (GtkMenuItem *) l->data;
        c = (gchar *) gtk_object_get_user_data (GTK_OBJECT (i));

        if (c && strcmp (c, label) == 0)
        {
            gtk_option_menu_set_history (o, j);
            break;
        }

        j++;
        l = g_list_next (l);
    }
}
