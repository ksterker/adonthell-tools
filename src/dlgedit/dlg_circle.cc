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
 * @file dlg_circle.cc
 *
 * @author Kai Sterker
 * @brief The node containing the actual dialogue text.
 */

#include "dlg_circle.h"
#include "gui_resources.h"

// Constructor
DlgCircle::DlgCircle (DlgPoint &p, node_type t, int nid, int mid, DlgCircleEntry *e)
{
    type_ = t;
    nid_ = nid;
    mid_ = mid;
    entry_ = (e ? e : new DlgCircleEntry);
    
    // Align Circle to the (imaginary) grid
    top_left = DlgPoint (p.x () - (p.x () % CIRCLE_DIAMETER), p.y () - (p.y () % CIRCLE_DIAMETER));
    bottom_right = DlgPoint (x () + CIRCLE_DIAMETER, y () + CIRCLE_DIAMETER);
    
    // make sure the point lies still within the circle
    p = center ();
}

// dtor
DlgCircle::~DlgCircle ()
{
    if (entry_) delete entry_;
}

// get a certain parent-circle of this circle
DlgCircle *DlgCircle::parent (query_type pos, int offset)
{
    DlgNode *arrow = prev (pos, offset);
    if (arrow != NULL) return (DlgCircle *) arrow->prev (FIRST);
    else return NULL;
}

// get a certain child-circle of this circle
DlgCircle *DlgCircle::child (query_type pos, int offset)
{
    DlgNode *arrow = next (pos, offset);
    if (arrow != NULL) return (DlgCircle *) arrow->next (FIRST);
    else return NULL;
}

// check whether the given node is a child of this circle
bool DlgCircle::hasChild (DlgNode *child)
{
    std::list<DlgNode*>::iterator j;
    
    for (j = next_.begin (); j != next_.end (); j++)
        if (child == (*j)->next (FIRST))
            return true;
    
    return false;
}

// draw the circle
void DlgCircle::draw (cairo_surface_t *surface, DlgPoint &os, GtkWidget *widget)
{
    cairo_t *cr = cairo_create (surface);

    // get the color and fill for drawing the circle
    const GdkColor *gc  = GuiResources::getColor  (mode_, type_);
    const GdkColor *fgc = GuiResources::getFill   (mode_, type_);
    
    // offset circle
    DlgPoint position = center ().offset (os);
    DlgPoint tmp = topLeft ().offset (os);
    DlgRect area (tmp, width () + 1, height () + 1);
    
    // draw everything to the surface - First border 
    drawCircle (cr, gc,  TRUE, position.x (), position.y (), CIRCLE_RADIUS);
    // add a small circle inside that indicates the type (NPC, Player)
    drawCircle (cr, fgc, TRUE, position.x (), position.y (), CIRCLE_RADIUS-2);

    // Indicate whether node contains additional code
    if (hasCode () || entry_->loop ())
    {
        GString *code = g_string_sized_new (2);
        
        if (hasCode ()) g_string_append_c (code, '!');
        if (entry_->loop ()) g_string_append_c (code, 'o');
        
        // get the font to use
        const PangoFontDescription *desc = pango_layout_get_font_description (GuiResources::font ());

        // create pango cairo compatible layout
        PangoLayout *font = pango_cairo_create_layout(cr);
        pango_layout_set_font_description (font, desc);
        pango_layout_set_text (font, code->str, -1);

        // place text in circles center
        int w, h;
        pango_layout_get_pixel_size (font, &w, &h);
        
        int x = tmp.x () + (width () - w) / 2;
        int y = tmp.y () + (height () + 1 - h) / 2;

        // set font color and position
        gdk_cairo_set_source_color(cr, gc);
        pango_cairo_update_layout (cr, font);
        cairo_move_to(cr, x, y);

        // draw text
        pango_cairo_show_layout (cr, font);
        g_object_unref(font);

        g_string_free (code, TRUE);
    }
    
    // Update the drawing area
    update (widget, area);

    cairo_destroy(cr);
}

// load a circle from a file
bool DlgCircle::load ()
{
    entry_ = new DlgCircleEntry;
    std::string str;
    int n;

    // as long as we haven't reached EOF or are finished with loading:
    while (1)
    {
        // look what we find in the file
        switch (parse_dlgfile (str, n))
        {
            // EOF or finished
            case 0:
            case LOAD_END: return true;

            // Type of node
            case LOAD_TYPE:
            {
                if (parse_dlgfile (str, n) == LOAD_NUM) type_ = (node_type) n;
                break;
            }
            
            // Loop
            case LOAD_LOOP:
            {
                if (parse_dlgfile (str, n) == LOAD_NUM) entry_->setLoop (n);
                break;
            }
            
            // Module and Node id of Circle
            case LOAD_ID:
            {
                if (parse_dlgfile (str, n) == LOAD_NUM) nid_ = n;
                
                break;
            }
            
            // Coordinates of Circle
            case LOAD_POS:
            {
                int px, py;
                if (parse_dlgfile (str, n) == LOAD_NUM) px = n;
                if (parse_dlgfile (str, n) == LOAD_NUM) py = n;

                // Align Circle to the (imaginary) grid
                top_left = DlgPoint (px - (px % CIRCLE_DIAMETER), py - (py % CIRCLE_DIAMETER));
                bottom_right = DlgPoint (x () + CIRCLE_DIAMETER, y () + CIRCLE_DIAMETER);
                
                break;
            }

            // The Circle's Text
            case LOAD_TEXT:
            {
                if (parse_dlgfile (str, n) == LOAD_STR) entry_->setText (str);
                break;
            }

            // The Circle's Annotations
            case LOAD_NOTE:
            {
                if (parse_dlgfile (str, n) == LOAD_STR) entry_->setAnnotation (str);
                break;
            }

            // The Circle's Character
            case LOAD_NPC:
            {
                if (parse_dlgfile (str, n) == LOAD_STR) entry_->setNpc (str);
                break;
            }

            // The Circle's Conditions
            case LOAD_COND:
            {
                if (parse_dlgfile (str, n) == LOAD_STR) entry_->setCondition (str);
                break;
            }

            // The Circle's Variables
            case LOAD_VARS:
            {
                if (parse_dlgfile (str, n) == LOAD_STR) entry_->setCode (str);
                break;
            }

            // Just ignore everything else
            default: break;
        }
    }

    return true;
}

// save all data neccessary to restore the circle
void DlgCircle::save (std::ofstream &file)
{
    // Keyword "Circle" and circle's number
    file << "\nCircle\n";

    // circle's type
    file << "  Type " << (int) type () << "\n";

    // circle's id
    file << "  Id " << nid_ << "\n";
        
    // circle's position
    file << "  Pos " << x () << " " << y () << "\n";

    // circle's annotation
    if (entry_->annotation () != "")
        file << "  Note �" << entry_->annotation () << "�\n";

    // circle's text
    file << "  Text �" << entry_->text () << "�\n";

    // circle's character
    if (type_ == NPC && entry_->npc () != "Default")
        file << "  NPC �" << entry_->npc () << "�\n";

    // circle's condition(s)
    if (entry_->condition () != "")
        file << "  Cond �" << entry_->condition () << "�\n";

    // circle's code
    if (entry_->code () != "")
        file << "  Vars �" << entry_->code () << "�\n";
    
    // loop
    if (entry_->loop () != false)
        file << "  Loop 1\n";
    
    file << "End\n";
}

// get the text of a circle
std::string DlgCircle::text ()
{
    std::string text = "";
    
    // is there any text at all?
    if (entry_ == NULL) return text;

    // is a certain NPC attached?
    if (type_ == NPC && entry_->npc () != "Default")
    {
        text += entry_->npc ();
        text += ": ";
    }
    
    // append actual text and return it
    return text += entry_->text ();
}

// get tooltip text of the circle
std::string DlgCircle::tooltip ()
{
    std::string text = "";
    
    // is there any text at all?
    if (entry_ == NULL) return text;

    // are there a one or more conditions?
    if (entry_->condition () != "")
    {
        text += entry_->condition ();
        text += "\n\n";
    }
    
    // append text
    text += DlgCircle::text ();
    
    // is there further code?
    if (entry_->code () != "")
    {
        text += "\n\n";
        text += entry_->code ();
    }
    
    return text;
}
