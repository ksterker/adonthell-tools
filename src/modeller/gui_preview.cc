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

#include <gfx/gfx.h>
#include <world/placeable_model.h>

#include "backend/gtk/screen_gtk.h"

#include "mdl_handle.h"
#include "gui_preview.h"

/// indices of text fields 
enum
{
    EDIT_OFFSET_X = 0,
    EDIT_OFFSET_Y = 1,
    EDIT_POS_X = 2,
    EDIT_POS_Y = 3,
    EDIT_POS_Z = 4,
    EDIT_SIZE_X = 5,
    EDIT_SIZE_Y = 6,
    EDIT_SIZE_Z = 7
};

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
    for (int i = EDIT_OFFSET_X; i <= EDIT_SIZE_Z; i++)
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
    
    // no handle selected
    SelectedHandle = -1;
    PrevPos = NULL;
    Model = NULL;
    Shape = NULL;
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
        Target->scale(tmp, base::Scale);
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

    if (Model != NULL)
    {
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
                    model->set_shape(Model->current_shape_name());
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
        
        // draw models
        Renderer.render (models, da, Target);
        
        // draw handles
        Renderer.render (Offset, Model, Handles, da, Overlay);
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
    SelectedHandle = -1;

    // update offset
    if (model != NULL)
    {
        setShapeData (EDIT_OFFSET_X, Model->current_shape()->ox());
        setShapeData (EDIT_OFFSET_Y, Model->current_shape()->oy());
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
    updateShapeData ();
    
    // no handle selected, initially
    SelectedHandle = -1;

    // update screen
    render ();
}

// notification of mouse movement
void GuiPreview::mouseMoved (const GdkPoint *point)
{
    if (Shape == NULL) return;
    
    // restrict drawing to view size
    gfx::drawing_area da (0, 0, Target->length()-1, Target->height()-1);
    // handle below cursor
    int curHandle = -1;
    
    for (int i = 0; i < MAX_HANDLES; i++)
    {
        GdkRectangle rect = { Handles[i].x, Handles[i].y, HANDLE_SIZE, HANDLE_SIZE };
        GdkRegion *region = gdk_region_rectangle (&rect);

        // printf ("[%i, %i] <-> [%i, %i]\n", Handles[i].x, Handles[i].y, point->x, point->y); 
        if (gdk_region_point_in (region, point->x, point->y))
        {
            curHandle = i;
            break;
        }
        
        // cleanup
        gdk_region_destroy (region);
    }

    // selection changed?
    if (curHandle == SelectedHandle) return;

    // was handle selected?
    if (SelectedHandle != -1)
    {
        // deselect handle
        Renderer.drawHandle (Handles[SelectedHandle], false, da, Overlay);
        // reset value
        indicateEditingField (SelectedHandle, false);
        // refresh screen
        GdkRectangle rect = { Handles[SelectedHandle].x, Handles[SelectedHandle].y, HANDLE_SIZE, HANDLE_SIZE };
        gdk_window_invalidate_rect (gtk_widget_get_window (DrawingArea), &rect, FALSE);
    }
            
    // is new handle selected?
    if (curHandle != -1)
    {
        // highlight handle
        Renderer.drawHandle (Handles[curHandle], true, da, Overlay);
        // highlight value
        indicateEditingField (curHandle, true);
        // refresh screen
        GdkRectangle rect = { Handles[curHandle].x, Handles[curHandle].y, HANDLE_SIZE, HANDLE_SIZE };
        gdk_window_invalidate_rect (gtk_widget_get_window (DrawingArea), &rect, FALSE);
    }
            
    // update handle
    SelectedHandle = curHandle;        
}

// indicate which value will be affected by dragging handle
void GuiPreview::indicateEditingField (const s_int32 & handle, const bool & highlight)
{
    int fields[] = { -1, -1, -1 };
    
    switch (handle)
    {
        case POSITION:
        {
            fields[0] = EDIT_POS_X;
            fields[1] = EDIT_POS_Y;
            fields[2] = EDIT_POS_Z; // with modifier key
            break;
        }
        case LENGTH:
        {
            fields[0] = EDIT_SIZE_X;
            break;
        }
        case WIDTH:
        {
            fields[0] = EDIT_SIZE_Y;
            break;
        }
        case HEIGHT:
        {
            fields[0] = EDIT_SIZE_Z;
            break;
        }
        default:
        {
            return;
        }
    }
    
    for (int i = 0; i < 3; i++)
    {
        if (fields[i] == -1) break;

        GtkStyle *style = NULL;
        if (highlight)
        {
            GdkColor color = { 0, 43760, 55745, 8315 };
            style = gtk_style_copy (gtk_widget_get_style (GTK_WIDGET (ShapeData[fields[i]])));
            style->base[GTK_STATE_NORMAL]   = color;
            style->base[GTK_STATE_ACTIVE]   = color;
            style->base[GTK_STATE_PRELIGHT] = color;
            style->base[GTK_STATE_SELECTED] = color;
        }
        
        gtk_widget_set_style (GTK_WIDGET (ShapeData[fields[i]]), style);    
    }
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
    
    // list of points that will get updated
    int points[world::cube3::NUM_CORNERS];
    // the offset by which the points will be moved
    world::vector3<s_int16> offset;
    
    switch (SelectedHandle)
    {
        case POSITION:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;

            if (modifier)
            {
                offset.set_z (PrevPos->y - point->y);
            }
            else
            {
                offset.set_x (point->x - PrevPos->x);
                offset.set_y (point->y - PrevPos->y);
            }
            break;
        }
        case LENGTH:
        {
            // move the points on the cube's right side (x axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[4] = -1;
            
            offset.set_x (point->x - PrevPos->x);
            break;
        }
        case WIDTH:
        {
            // move the points on the cube's back side (y axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_BACK_LEFT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = -1;

            offset.set_y (point->y - PrevPos->y);
            break;
        }
        case HEIGHT:
        {
            // move the points on the cube's bottom side (z axis)
            points[0] = world::cube3::TOP_FRONT_LEFT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::TOP_BACK_RIGHT;
            points[3] = world::cube3::TOP_BACK_LEFT;
            points[4] = -1;
            
            offset.set_z (PrevPos->y - point->y);
            break;
        }
        default:
        {
            // no handle selected
            return;
        }
    }
    
    // take care of scaling
    offset = offset * (1.0f / base::Scale);

    // handle small movements in scaled mode
    if (offset.squared_length() == 0) return;

    // now update the selected points
    for (int i = 0; i < world::cube3::NUM_CORNERS; i++)
    {
        if (points[i] == -1) break;
        
        world::vector3<s_int16> curPos = Shape->get_point (points[i]);
        Shape->set_point (points[i], curPos + offset);
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
    updateShapeData ();
    
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
    for (int i = 0; i < 8; i++)
    {
        // find the entry we just edited
        if (ShapeData[i] == entry)
        {
            pos = i;
            break;
        }
    }
    
    // list of points that will get updated
    int points[world::cube3::NUM_CORNERS];
    // the offset by which the points will be moved    
    world::vector3<s_int16> offset;

    switch (pos)
    {
        case EDIT_OFFSET_X:
        {
            world::placeable_shape* shape = Model->current_shape ();
            shape->set_offset (value, shape->oy());
            render ();
            return;
        }
        case EDIT_OFFSET_Y:
        {
            world::placeable_shape* shape = Model->current_shape ();
            shape->set_offset (shape->ox(), value);
            render ();
            return;
        }
        case EDIT_POS_X:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;

            offset.set_x (value - Shape->min_x());
            break;
        }
        case EDIT_POS_Y:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;
            
            offset.set_y (value - Shape->min_y());
            break;
        }
        case EDIT_POS_Z:
        {
            // move the whole cube
            points[0] = world::cube3::BOTTOM_FRONT_LEFT;
            points[1] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = world::cube3::TOP_FRONT_LEFT;
            points[5] = world::cube3::TOP_FRONT_RIGHT;
            points[6] = world::cube3::TOP_BACK_RIGHT;
            points[7] = world::cube3::TOP_BACK_LEFT;
            
            offset.set_z (value - Shape->min_z());
            break;
        }
        case EDIT_SIZE_X:
        {
            // move the points on the cube's right side (x axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_FRONT_RIGHT;
            points[4] = -1;
            
            offset.set_x (value - (Shape->max_x() - Shape->min_x()));
            break;
        }
        case EDIT_SIZE_Y:
        {
            // move the points on the cube's back side (y axis)
            points[0] = world::cube3::TOP_BACK_RIGHT;
            points[1] = world::cube3::TOP_BACK_LEFT;
            points[2] = world::cube3::BOTTOM_BACK_RIGHT;
            points[3] = world::cube3::BOTTOM_BACK_LEFT;
            points[4] = -1;
            
            offset.set_y (value - (Shape->max_y() - Shape->min_y()));
            break;
        }
        case EDIT_SIZE_Z:
        {
            // move the points on the cube's bottom side (z axis)
            points[0] = world::cube3::TOP_FRONT_LEFT;
            points[1] = world::cube3::TOP_FRONT_RIGHT;
            points[2] = world::cube3::TOP_BACK_RIGHT;
            points[3] = world::cube3::TOP_BACK_LEFT;
            points[4] = -1;
            
            offset.set_z (value - (Shape->max_z() - Shape->min_z()));
            break;
        }
        default: return;
    }
    
    // check if there is any modification at all
    if (offset.squared_length() == 0) return;

    // set modified state
    setModified();

    // now update the selected points
    for (int i = 0; i < world::cube3::NUM_CORNERS; i++)
    {
        if (points[i] == -1) break;
        
        world::vector3<s_int16> curPos = Shape->get_point (points[i]);
        Shape->set_point (points[i], curPos + offset);
    }
    
    // update the bounding box and model
    Model->current_shape ()->remove_part (Shape);
    Shape->create_bounding_box ();
    Model->current_shape ()->add_part (Shape);
    
    // and finally the screen
    render ();    
}

// display size and position of current shape
void GuiPreview::updateShapeData () const
{
    if (Shape != NULL)
    {
        setShapeData (EDIT_POS_X, Shape->min_x());
        setShapeData (EDIT_POS_Y, Shape->min_y());
        setShapeData (EDIT_POS_Z, Shape->min_z());
        setShapeData (EDIT_SIZE_X, Shape->max_x() - Shape->min_x());
        setShapeData (EDIT_SIZE_Y, Shape->max_y() - Shape->min_y());
        setShapeData (EDIT_SIZE_Z, Shape->max_z() - Shape->min_z());
    }
    else
    {
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_POS_X]), "");
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_POS_Y]), "");
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_POS_Z]), "");
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_SIZE_X]), "");
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_SIZE_Y]), "");
        gtk_entry_set_text (GTK_ENTRY(ShapeData[EDIT_SIZE_Z]), "");
    }
}

// set shape data
void GuiPreview::setShapeData (const u_int32 & data, const s_int32 & value) const
{
    std::stringstream strval (std::ios::out);
    strval << value;
    
    gtk_entry_set_text (GTK_ENTRY(ShapeData[data]), strval.str().c_str());
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
