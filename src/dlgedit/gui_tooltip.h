/*
   $Id: gui_tooltip.h,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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
 * @file gui_tooltip.h
 *
 * @author Kai Sterker
 * @brief A custom, tooltip-like widget
 */

#ifndef GUI_TOOLTIP_H
#define GUI_TOOLTIP_H

#include "dlg_node.h"

/**
 * A widget similar to the GtkTooltip that is used to display a
 * DlgCircle's text as long as the mouse hovers over the circle.
 */
class GuiTooltip
{
public:
    GuiTooltip (DlgNode *node);
    ~GuiTooltip ();

    void draw (GtkWidget *parent, DlgPoint &offset);
    
private:
    DlgNode *node;
    GtkWidget *tooltip;
};

#endif // GUI_TOOLTIP_H
