/*
 Copyright (C) 2009 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file modeller/gui_preview.cc
 *
 * @author Kai Sterker
 * @brief Where the model is displayed and assembled.
 */

#include <gtk/gtk.h>

#include <adonthell/gfx/gfx.h>
#include <adonthell/world/placeable_model.h>

#include "backend/gtk/screen_gtk.h"

#include "mdl_handle.h"
#include "gui_preview.h"

#define EDIT_OFFSET_X 0
#define EDIT_OFFSET_Y 1
#define NUM_EDIT_FIELDS 8

// Window resized
static gint configure_event (GtkWidget *widget, GdkEventConfigure *event, gpointer data)
{
    GuiPreview *view = (GuiPreview *) data;
    
    // resize the drawing area
    view->resizeSurface (widget);
    
    return TRUE;
}

// Redraw the screen from the backing pixmap
static gint expose_event (GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    GuiPreview *view = (GuiPreview *) data;
    
    // redraw part of the screen that has changed
    view->draw(event->area.x, event->area.y, event->area.width, event->area.height);
    
    return TRUE;
}

// Mouse moved over drawing area
gint motion_notify_event (GtkWidget *widget, GdkEventMotion *event, gpointer data)
{
    GuiPreview *view = (GuiPreview *) data;
    GdkPoint point = { event->x, event->y };
    
    // scroll the preview, if necessary
    view->prepareScrolling (point);

    // drag handle?
    if (event->state & GDK_BUTTON1_MASK)
    {
        view->handleDragged (&point, event->state & GDK_SHIFT_MASK);
    }
    else
    {
        if (view->isHandleDragged())
        {
            view->stopDragging();
        }

        // highlight handles under cursor
        view->mouseMoved (&point);
    }

    if (event->is_hint)
    {
        gdk_event_request_motions (event);
    }

    return FALSE;
}

// Mouse button pressed on drawing area
gint button_press_event (GtkWidget *widget, GdkEventButton *event, gpointer data)
{
    // Left button pressed
    if (event->button == 1)
    {
        GuiPreview *view = (GuiPreview *) data;
        GdkPoint point = { event->x, event->y };
        view->startDragging (&point);
    }

    return false;
}

// direct editing of shape
static void on_number_changed (GtkEditable *editable, gpointer data) 
{
    GuiPreview *view = (GuiPreview *) data;
    view->updateShape (GTK_ENTRY(editable));
}

// ctor
GuiPreview::GuiPreview (GtkWidget *drawing_area, GtkEntry** shape_data, GtkTreeModel *models) 
: DrawingArea (drawing_area), ShapeData (shape_data), ModelList (models)
{
#ifdef __APPLE__
    // no need to use double buffering on OSX, but appears to be required elsewhere
    GTK_WIDGET_UNSET_FLAGS (GTK_WIDGET (DrawingArea), GTK_DOUBLE_BUFFERED);
#endif

    g_signal_connect (G_OBJECT (DrawingArea), "expose_event", G_CALLBACK(expose_event), this);
    g_signal_connect (G_OBJECT (DrawingArea), "configure_event", G_CALLBACK(configure_event), this);
    g_signal_connect (G_OBJECT (DrawingArea), "motion_notify_event", G_CALLBACK(motion_notify_event), this);
    g_signal_connect (G_OBJECT (DrawingArea), "button_press_event", G_CALLBACK(button_press_event), this);
    // g_signal_connect (G_OBJECT (GuiMapedit::window->getWindow ()), "key_press_event", G_CALLBACK(key_press_notify_event), this);
    
    gtk_widget_set_events (DrawingArea, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK);

    // callbacks for directly editing shape
    for (int i = 0; i < NUM_EDIT_FIELDS; i++)
    {
        g_signal_connect (G_OBJECT (shape_data[i]), "changed", G_CALLBACK(on_number_changed), this); 
    }
    
    // scroll offset
    Offset.x = 0;
    Offset.y = 0;

    // create the render target
    Target = gfx::create_surface();    

    // create UI overlay
    Overlay = gfx::create_surface();
    Overlay->set_alpha(255, true);
    
    // no editor, model and no shape yet
    PrevPos = NULL;
    Editor = NULL;
    Model = NULL;
    Shape = NULL;
    Connector = NULL;
}

// redraw the given part of the screen
void GuiPreview::draw (const int & sx, const int & sy, const int & l, const int & h)
{
    // get "screen" surface 
    gfx::screen_surface_gtk *s = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    s->set_drawable (gtk_widget_get_window (DrawingArea));
    
    // set clipping rectangle
    gfx::drawing_area da (sx, sy, l, h);
    
    // zoom stuff (testing)
    if (base::Scale != 1)
    {
        int x = Offset.x - X_AXIS_POS * (base::Scale - 1);
        int y = Offset.y - (Target->height() / 2) * (base::Scale - 1);

        gfx::surface *tmp = gfx::create_surface();
        tmp->resize(Target->length() * base::Scale, Target->height() * base::Scale);
        Target->scale_up(tmp, base::Scale);
        tmp->draw (x, y, &da, s);
        delete tmp;
    }
    else
    {
        // draw target
        Target->draw (0, 0, &da, s);
    }

    Overlay->draw(0, 0, &da, s);
}

// update size of the view
void GuiPreview::resizeSurface (GtkWidget *widget)
{
    GtkAllocation size;
    gtk_widget_get_allocation (DrawingArea, &size);

    // set size of the render target
    Target->resize (size.width, size.height);
    Overlay->resize (size.width, size.height);
    
    // update screen
    render ();
}    

// update screen
void GuiPreview::render ()
{
    GtkAllocation size;
    gtk_widget_get_allocation (DrawingArea, &size);

    render (0, 0, size.width, size.height);
}

// update part of the screen
void GuiPreview::render (const int & sx, const int & sy, const int & l, const int & h)
{
    // reset target before drawing
    Target->fillrect (sx, sy, l, h, 0xFF000000);
    Overlay->fillrect (sx, sy, l, h, 0x00000000);

    // set clipping rectangle
    gfx::drawing_area da (sx, sy, l, h);

    // render x and y axis
    u_int32 color = Overlay->map_color (0x40, 0x40, 0x40);
    Overlay->draw_line (0, Offset.y + Overlay->height()/2, Overlay->length(), Offset.y + Overlay->height()/2, color, &da);
    Overlay->draw_line (Offset.x + X_AXIS_POS, 0, Offset.x + X_AXIS_POS, Overlay->height(), color, &da);

    // collect all the sprites we have for rendering
    GtkTreeIter iter;
    std::list<world::render_info> models;

    if (gtk_tree_model_get_iter_first (ModelList, &iter))
    {
        world::placeable_model *model = NULL;
        std::vector<world::shadow_info> shadow;

        do
        {
            gtk_tree_model_get (ModelList, &iter, 1, &model, -1);
            if (model != NULL)
            {
                model->set_shape(Model ? Model->current_shape_name() : "");

                gfx::sprite *sprt = model->get_sprite();
                if (sprt != NULL)
                {
                    // information required for rendering
                    world::render_info ri (model->current_shape(), sprt, world::vector3<s_int32>(), &shadow);
                    models.push_back (ri);
                }
            }
        }
        while (gtk_tree_model_iter_next (ModelList, &iter));
    }

    if (Model == NULL)
    {
        // draw ground rectangle and selected connector
        // needs to happen before drawing the models, because that
        // will change the list of models.
        Renderer.render (Offset, models, Connector, da, Overlay);
    }

    // draw models
    Renderer.render (models, da, Target);

    if (Model != NULL)
    {
        // draw handles
        Renderer.render (Offset, Model, Editor->getHandles(), da, Overlay);
    }

    // schedule screen update
    GdkRectangle rect = { sx, sy, l, h };
    gdk_window_invalidate_rect (gtk_widget_get_window (DrawingArea), &rect, FALSE);
}

// set object being edited
void GuiPreview::setCurModel (world::placeable_model *model)
{
    Renderer.setActiveModel (model);

    // remember model for updates
    Model = model;

    // no handle selected, initially
    Editor->getHandles()->clearSelection();

    // update offset
    if (model != NULL)
    {
        Editor->setShapeData (GTK_ENTRY(ShapeData[EDIT_OFFSET_X]), Model->current_shape()->ox());
        Editor->setShapeData (GTK_ENTRY(ShapeData[EDIT_OFFSET_Y]), Model->current_shape()->oy());
    }
    else
    {
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_OFFSET_X]), "");
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_OFFSET_Y]), "");
    }
    
    // update screen
    render ();
}

// set shape being edited
void GuiPreview::setCurShape (world::cube3 *shape)
{
    Renderer.setActiveShape (shape);

    // remember shape for updates
    Shape = shape;
    
    // display shape extends and position
    Editor->updateShapeData (ShapeData, Shape);
    
    // no handle selected, initially
    Editor->getHandles()->clearSelection();

    // update screen
    render ();
}

// set connector being edited
void GuiPreview::setCurConnector(MdlConnector *connector)
{
    Connector = connector;

    // update screen
    render ();
}

// notification of mouse movement
void GuiPreview::mouseMoved (const GdkPoint *point)
{
    if (Shape == NULL) return;
    
    // restrict drawing to view size
    gfx::drawing_area da (0, 0, Target->length()-1, Target->height()-1);
    // the handles for the current editor
    ModelHandles *handles = Editor->getHandles();
    // store current handle
    int curHandle = handles->selected();
    
    // selection changed?
    if (!handles->updateSelection (point)) return;

    // was handle selected before?
    if (curHandle != -1)
    {
        // deselect handle
        Renderer.drawHandle ((*handles)[curHandle], false, da, Overlay);
        // reset value
        Editor->indicateEditingField (ShapeData, curHandle, false);
        // refresh screen
        GdkRectangle rect = { (*handles)[curHandle].x, (*handles)[curHandle].y, HANDLE_SIZE, HANDLE_SIZE };
        gdk_window_invalidate_rect (gtk_widget_get_window (DrawingArea), &rect, FALSE);
    }
            
    // is handle selected now?
    if (handles->selected() != -1)
    {
        // highlight handle
        Renderer.drawHandle ((*handles)[handles->selected()], true, da, Overlay);
        // highlight value
        Editor->indicateEditingField (ShapeData, handles->selected(), true);
        // refresh screen
        GdkRectangle rect = { (*handles)[handles->selected()].x, (*handles)[handles->selected()].y, HANDLE_SIZE, HANDLE_SIZE };
        gdk_window_invalidate_rect (gtk_widget_get_window (DrawingArea), &rect, FALSE);
    }

    // display new data related to selected handle
    Editor->updateShapeData (ShapeData, Shape);
}

// set initial position from which we start dragging
void GuiPreview::startDragging (GdkPoint *point)
{
    if (PrevPos == NULL)
    {
        PrevPos = new GdkPoint();
        PrevPos->x = point->x;
        PrevPos->y = point->y;
    }
}

// update shape size or position
void GuiPreview::handleDragged (GdkPoint *point, const bool & modifier)
{
    // set initial position, if not available yet
    startDragging (point);

    // any change at all?
    if (PrevPos->x == point->x && PrevPos->y == point->y) 
    {
        return;
    }

    GdkPoint distance = { point->x - PrevPos->x, point->y - PrevPos->y };
    if (!Editor->handleDragged (Shape, &distance, modifier))
    {
        return;
    }
    
    // set modified state
    setModified();

    // remember position for next iteration
    PrevPos->x = point->x;
    PrevPos->y = point->y;
    
    // update the bounding box and model
    Model->current_shape ()->remove_part (Shape);
    Shape->create_bounding_box ();
    Model->current_shape ()->add_part (Shape);
    
    // display new size and position
    Editor->updateShapeData (ShapeData, Shape);
    
    // and finally the screen
    render ();
}

// update shape from direct text entry
void GuiPreview::updateShape (GtkEntry *entry)
{
    if (Shape == NULL) return;
     
    // do we have a valid value
    int value;
    std::stringstream in (gtk_entry_get_text (entry));        
    in >> std::skipws >> value; 
    if (in.fail ()) return;
    
    int pos = -1;
    for (int i = 0; i < NUM_EDIT_FIELDS; i++)
    {
        // find the entry we just edited
        if (ShapeData[i] == entry)
        {
            pos = i;
            break;
        }
    }
    
    switch (pos)
    {
        case EDIT_OFFSET_X:
        {
            world::placeable_shape* shape = Model->current_shape ();
            shape->set_offset (value, shape->oy());
            break;
        }
        case EDIT_OFFSET_Y:
        {
            world::placeable_shape* shape = Model->current_shape ();
            shape->set_offset (shape->ox(), value);
            break;
        }
        default:
        {
            // apply modification to shape ...
            if (Editor->updateShape (Shape, pos, value))
            {
                // ... then update the bounding box and model
                Model->current_shape ()->remove_part (Shape);
                Shape->create_bounding_box ();
                Model->current_shape ()->add_part (Shape);
                break;
            }
            else return;
        }
    }

    // update modified state
    setModified();
    
    // and finally the screen
    render ();    
}

void GuiPreview::setModified()
{
    GtkWidget *toplevel = gtk_widget_get_toplevel (DrawingArea);
    if (gtk_widget_is_toplevel (toplevel))
    {
        std::string title (gtk_window_get_title (GTK_WINDOW (toplevel)));
        size_t len = title.length();
        if (len > 1 && title[len-2] != '*')
        {
            title.insert (len-1, "*");
            gtk_window_set_title (GTK_WINDOW (toplevel), title.c_str());
        }
    }
}

// perform scrolling
void GuiPreview::scroll()
{
    GdkPoint newOffset =
    {
        Offset.x + scroll_offset.x,
        Offset.y + scroll_offset.y
    };

    if (newOffset.x > 0)
    {
        newOffset.x = 0;
    }
    else if (newOffset.x < (X_AXIS_POS - Target->length()) * (base::Scale -1))
    {
        newOffset.x = (X_AXIS_POS - Target->length()) * (base::Scale - 1);
    }

    if (newOffset.y < 0)
    {
        newOffset.y = 0;
    }
    else if (newOffset.y > Target->height()/2 * (base::Scale -1))
    {
        newOffset.y = Target->height()/2 * (base::Scale - 1);
    }

    if (newOffset.x != Offset.x || newOffset.y != Offset.y)
    {
        Offset = newOffset;
        render ();
    }
}
