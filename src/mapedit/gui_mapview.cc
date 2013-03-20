/*
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
 * @file mapedit/gui_mapview.cc
 *
 * @author Kai Sterker
 * @brief Screen for the map.
 */

#include <gdk/gdk.h>
#include <gtk/gtk.h>

#include <adonthell/gfx/gfx.h>
#include "backend/gtk/screen_gtk.h"

#include "gui_entity_dialog.h"
#include "gui_entity_list.h"
#include "gui_filter_dialog.h"
#include "gui_grid.h"
#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "gui_mapview_events.h"
#include "gui_zone.h"
#include "map_data.h"
#include "map_entity.h"
#include "map_mgr.h"

// ctor
GuiMapview::GuiMapview(GtkWidget *paned)
{
    // create drawing area for the graph
    Screen = gtk_drawing_area_new ();

    // height control
    RenderHeight = new GuiRenderHeight (); 
    
#ifdef __APPLE__
    // no need to use double buffering on OSX, but appears to be required elsewhere
    GTK_WIDGET_UNSET_FLAGS (GTK_WIDGET (Screen), GTK_DOUBLE_BUFFERED);
#endif
    
    GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (hbox);
    gtk_box_pack_start (GTK_BOX(hbox), Screen, TRUE, TRUE, 0);
    gtk_box_pack_start (GTK_BOX(hbox), RenderHeight->widget(), FALSE, TRUE, 0);
    
    gtk_widget_set_size_request (GTK_WIDGET (Screen), 800, 600);
    gtk_paned_add2 (GTK_PANED (paned), hbox);
    gtk_widget_show (Screen);
    gtk_widget_grab_focus (Screen);
    
    // register our event callbacks
    g_signal_connect (G_OBJECT (Screen), "expose_event", G_CALLBACK(expose_event), this);
    g_signal_connect (G_OBJECT (Screen), "configure_event", G_CALLBACK(configure_event), this);
    g_signal_connect (G_OBJECT (Screen), "motion_notify_event", G_CALLBACK(motion_notify_event), this);
    g_signal_connect (G_OBJECT (Screen), "button_press_event", G_CALLBACK(button_press_event), this);
    g_signal_connect (G_OBJECT (GuiMapedit::window->getWindow ()), "key_press_event", G_CALLBACK(key_press_notify_event), this);
    g_signal_connect (G_OBJECT (GuiMapedit::window->getWindow ()), "key_release_event", G_CALLBACK(key_release_notify_event), this);
    g_signal_connect (G_OBJECT (RenderHeight->widget()), "value-changed", G_CALLBACK(on_renderheight_changed), this);
    
    gtk_widget_set_events (Screen, GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK |
                           GDK_POINTER_MOTION_MASK | GDK_KEY_PRESS_MASK | GDK_KEY_RELEASE_MASK);

    // create view
    View = world::area_manager::get_mapview ();
    View->set_renderer (&Renderer);
    View->resize (800, 600);
    
    // create the render target
    Target = gfx::create_surface();
    
    // create the overlay
    Overlay = gfx::create_surface ();
    Overlay->set_alpha (255, true);
    
    // create the grid
    Grid = new GuiGrid (Overlay);
    Zones = new GuiZone (Overlay);
    
    // Memeber intialization
    CurObj = NULL;
    DrawObj = NULL;
    CandidateObj = NULL;
}

// dtor
GuiMapview::~GuiMapview()
{
    delete Target;
    delete Overlay;
    delete Grid;
}

// set map to render
void GuiMapview::setMap (MapData *area)
{
    MapMgr::set_map (area);
    
    View->set_position (area->x(), area->y(), area->z());
    
    // display map coordinates of mouse pointer
    updateLocation (area);
    
    // update valid height range
    RenderHeight->setMapExtend(area->min().z(), area->max().z());
    
    // draw map
    render();
}

void GuiMapview::zoom ()
{
    // redraw the overlay
    updateOverlay();

    // update paint object
    if (DrawObj != NULL)
    {
        MapEntity *tmp = DrawObj;
        DrawObj = NULL;
        delete DrawObjSurface;
        selectObj (tmp);
    }

    // update view
    render ();

    // update map coordinates of mouse pointer
    updateLocation ((MapData*) MapMgr::get_map());
}

// redraw the whole screen
void GuiMapview::draw ()
{
    GtkAllocation allocation;
    gtk_widget_get_allocation (Screen, &allocation);

    draw (0, 0, allocation.width, allocation.height);
    
    GdkRectangle rect = { 0, 0, allocation.width, allocation.height };
    gdk_window_invalidate_rect (gtk_widget_get_window (Screen), &rect, FALSE);
}

// redraw the given part of the screen
void GuiMapview::draw (const int & sx, const int & sy, const int & l, const int & h)
{
    // get "screen" surface 
    gfx::screen_surface_gtk *s = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    s->set_drawable (gtk_widget_get_window (Screen));
    
    // set clipping rectangle
    gfx::drawing_area da (sx, sy, l, h);
    
    if (base::Scale > 1)
    {
        // draw target with zoom factor
        gfx::surface *tmp = gfx::create_surface();
        tmp->resize(Target->length(), Target->height());
        Target->scale_up(tmp, base::Scale);
        tmp->draw (0, 0, &da, s);
        delete tmp;
    }
    else
    {
        // draw mapview
        Target->draw (0, 0, &da, s);
    }
    
    // draw overlay
    Overlay->draw (0, 0, &da, s);
}

// render the whole map view
void GuiMapview::render ()
{
    GtkAllocation allocation;
    gtk_widget_get_allocation (Screen, &allocation);

    render (0, 0, allocation.width / base::Scale, allocation.height / base::Scale);
}

// render part of the map view
void GuiMapview::render (const int & sx, const int & sy, const int & l, const int & h)
{
    // reset target before drawing
    Target->fillrect (sx, sy, l, h, 0xFF000000);
    
    // is there a map attached to the view at all?
    MapData *area = (MapData*) MapMgr::get_map();
    if (area != NULL)
    {
        // update position of map
        View->set_position (area->x() + sx, area->y() + sy, area->z());
        
        // render mapview to screen
        View->resize (l, h);
        View->draw (sx, sy, NULL, Target);        
    }
    
    // schedule screen update
    GdkRectangle rect = { sx * base::Scale, sy * base::Scale, l * base::Scale, h * base::Scale };
    gdk_window_invalidate_rect (gtk_widget_get_window (Screen), &rect, FALSE);
}

// render specific object at given offset
void GuiMapview::renderObject (world::chunk_info *obj)
{
    if (obj != NULL) 
    {
        // get object bbox
        int x, y, l, h;
        getObjectExtend (obj, x, y, l, h);
        
        // get map offset
        MapData *area = (MapData*) MapMgr::get_map();
        
        // draw
        render (x - area->x(), y - area->y() + area->z(), l, h);
    }    
}

// redraw the overlay
void GuiMapview::updateOverlay()
{
    GtkAllocation allocation;
    gtk_widget_get_allocation (Screen, &allocation);

    // clear overlay
    Overlay->fillrect(0, 0, allocation.width, allocation.height, 0);
    // redraw grid
    Grid->draw();
    // redraw zones
    Zones->update();
}

// update size of the view
void GuiMapview::resizeSurface (GtkWidget *widget)
{
    GtkAllocation allocation;
    gtk_widget_get_allocation (widget, &allocation);

    // set the size of the actual map view
    View->resize (allocation.width, allocation.height);
    
    // set size of the map view render target
    Target->resize (allocation.width, allocation.height);
    
    // set the size of the overlay
    Overlay->resize (allocation.width, allocation.height);

    // redraw the overlay
    updateOverlay ();

    // redraw map view
    render ();
}

// notification of mouse movement
void GuiMapview::mouseMoved (const GdkPoint * point)
{
    MapData *area = (MapData*) MapMgr::get_map();
    GdkPoint scaled = { point->x, point->y };

    if (base::Scale > 1)
    {
        scaled.x = scaled.x / base::Scale;
        scaled.y = scaled.y / base::Scale;
    }

    if (area != NULL)
    {
        int ox = area->x();
        int oy = area->y();
        int oz = area->z();

        // display map coordinates of mouse pointer
        updateLocation(area);

        if (DrawObj == NULL)
        {
            GdkPoint p = { scaled.x + ox, scaled.y + oy - oz };

            // find object that's being moused over
            std::list<world::chunk_info*> objects_under_mouse;
            area->objects_in_view (p.x, p.x, p.y, p.y, objects_under_mouse);
            std::list<world::chunk_info*>::iterator i = objects_under_mouse.begin();
            while (i != objects_under_mouse.end())
            {
                if ((*i)->Min.z() > RenderHeight->getLimit())
                {
                    i = objects_under_mouse.erase(i);
                    continue;
                }
                i++;
            }
            world::chunk_info *obj = Renderer.findObjectBelowCursor (ox, oy - oz, &p, objects_under_mouse);
            
            // no object below cursor
            if (obj == NULL)
            {
                // reset previously highlighted object
                if (CurObj != NULL)
                {
                    renderObject (CurObj->getLocation());
                }
                
                CurObj = NULL;
                return;
            }
 
            // is object different from currently highlighted object?
            if (CurObj == NULL || obj != CurObj->getLocation())
            {
                // reset previously highlighted object
                if (CurObj != NULL)
                {
                    renderObject (CurObj->getLocation());
                }
                
                // keep track of highlighted object
                CurObj = GuiMapedit::window->entityList()->findEntity (obj->get_entity());
                if (CurObj != NULL)
                {
                    CurObj->setLocation (obj);
                    // highlight new object
                    renderObject (obj);
                }
                else
                {
                    // TODO: show error in status bar
                    fprintf (stderr, "*** mouseMoved: cannot find entity in entity list!\n");
                }
            }
        }
        else
        {            
            // get object height
            world::placeable *obj = DrawObj->object();
            int h = obj->cur_z() + obj->height();

            // scaled position
            int sx = DrawObjPos.x() * base::Scale;
            int sy = DrawObjPos.y() * base::Scale;

            GdkRectangle rect1 = { sx, sy, DrawObjSurface->length(), DrawObjSurface->height() };
            GdkRegion *region = gdk_region_rectangle (&rect1);
            
            // erase at previous position
            Overlay->fillrect (sx, sy, DrawObjSurface->length(), DrawObjSurface->height(), 0x0);
            Grid->draw (sx, sy, DrawObjSurface->length(), DrawObjSurface->height());
            Zones->draw (sx, sy, DrawObjSurface->length(), DrawObjSurface->height());

            // store new position
            DrawObjPos = Grid->align_to_grid (world::vector3<s_int32> (scaled.x, scaled.y, oz));
            DrawObjPos.set_y (DrawObjPos.y() - h);

            GdkRectangle rect2 = { DrawObjPos.x() * base::Scale, DrawObjPos.y() * base::Scale, DrawObjSurface->length(), DrawObjSurface->height() };
            gdk_region_union_with_rect (region, &rect2);
            
            // draw at new position
            DrawObjSurface->draw (DrawObjPos.x() * base::Scale, DrawObjPos.y() * base::Scale, NULL, Overlay);
            
            // check for overlap with objects already on the map
            indicateOverlap ();
            
            // blit to screen
            gdk_window_invalidate_region (gtk_widget_get_window (Screen), region, FALSE);
            gdk_region_destroy (region);
        }
    }
}

// add red tint if object would overlap other objects on the map
void GuiMapview::indicateOverlap ()
{
    static world::vector3<s_int32> V1(1,1,1);

    MapData *area = (MapData*) MapMgr::get_map();        
    world::vector3<s_int32> area_pos (area->x(), area->y(), 0); // DrawObj is already located at z-position of area
    
    const world::placeable *obj = DrawObj->object();
    int h = obj->cur_z() + obj->height();
    
    world::vector3<s_int32> min = DrawObjPos + area_pos + obj->entire_min();
    min.set_y (min.y() + h); // this is the offset subtracted when creating DrawObjPos
    world::vector3<s_int32> max = min + obj->entire_max();

    world::named_entity e (DrawObj->object(), "tmp", false);
    world::chunk_info ci (&e, min + V1, max - V1);
    
    // get all objects that possibly intersect with our object
    std::list<world::chunk_info*> objs_on_map = area->objects_in_bbox (ci.Min, ci.Max);
    if (DrawObj->intersects(objs_on_map, ci.center_min() - V1))
    {
        gfx::surface *tint = gfx::create_surface ();
        tint->set_alpha (128);
        tint->resize (DrawObjSurface->length(), DrawObjSurface->height());
        tint->fillrect (0, 0, DrawObjSurface->length(), DrawObjSurface->height(), 0xFFFF0000);
        tint->draw (DrawObjPos.x() * base::Scale, DrawObjPos.y() * base::Scale, NULL, Overlay);
        delete tint;
    }
}

// update map coordinate display
void GuiMapview::updateLocation(MapData *area)
{
    int x, y;
    GtkAllocation allocation;

    gtk_widget_get_pointer (Screen, &x, &y);
    gtk_widget_get_allocation (Screen, &allocation);

    if (base::Scale > 1)
    {
        x = x / base::Scale;
        y = y / base::Scale;
    }

    // if grid is active, show position where object would be placed
    // on the map instead of the raw cursor position.
    world::vector3<s_int32> v (x, y, area->z());
    if (DrawObj != NULL)
    {
        v = Grid->align_to_grid (v);
    }

    GuiMapedit::window->setLocation(v.x() + area->x(), v.y() + area->y(), v.z());
}

// change height
void GuiMapview::updateHeight (const s_int16 & oz)
{
    MapData *area = (MapData*) MapMgr::get_map();
    
    if (area != NULL)
    {
        area->setZ (area->z() + oz);
        
        // update overlap indicator
        if (DrawObj != NULL)
        {
            highlightObject ();
        }
        else
        {
            updateLocation(area);
        }
        
        // redraw
        render();
    }
}

// set height to object pos
void GuiMapview::zFromCurObj (const bool & top)
{
    MapData *area = (MapData*) MapMgr::get_map();

    if (area != NULL && CurObj != NULL)
    {
        world::chunk_info *pos = CurObj->getLocation();
        s_int32 new_z = top ? pos->Max.z() : pos->Min.z();
        updateHeight(new_z - area->z());
    }
}

// change render limit
void GuiMapview::updateRenderHeight (const s_int32 & limit)
{
    // update mapview with the limit
    View->limit_z (limit);
    
    // update view
    render();
}

// start tabbing through the entity list
void GuiMapview::startSelection (const bool & backwards)
{
    if (backwards)
    {
        CandidateObj = GuiMapedit::window->entityList()->getPrev();
    }
    else
    {
        CandidateObj = GuiMapedit::window->entityList()->getNext();
    }

    if (CandidateObj != NULL)
    {
        // preview candidate object
        selectObj (CandidateObj);
    }
}

// finish tabbing through the entity list
void GuiMapview::finishSelection ()
{
    if (CandidateObj != NULL)
    {
        GuiMapedit::window->entityList()->setSelected (CandidateObj);
        CandidateObj = NULL;
    }
}

// pick currently highlighted object for drawing
void GuiMapview::selectCurObj ()
{
    if (CurObj != NULL)
    {
        // try to select new object even with filter enabled
        if (GuiMapedit::window->entityList()->setSelected (CurObj))
        {
            return;
        }

        // temporarily disable filter, so we can select any map object
        GuiFilterDialog::pauseFiltering();

        if (!GuiMapedit::window->entityList()->setSelected (CurObj))
        {
            fprintf (stderr, "selectCurObj: %s not found in entity list\n", CurObj->get_name());
        }

        // resume filtering
        GuiFilterDialog::resumeFiltering();
    }
}

// use given object for drawing
void GuiMapview::selectObj (MapEntity *ety)
{
    // already selected?
    if (DrawObj == ety) return;
    
    // cleanup previously selected object
    if (DrawObj != NULL)
    {
        delete DrawObjSurface;
        DrawObjSurface = NULL;
        DrawObj = NULL;
    }

    DrawObj = ety;
    MapData *area = (MapData*) MapMgr::get_map();

    // build a fake location
    world::placeable *obj = ety->object();
    world::named_entity e (obj, "tmp", false);
    world::vector3<s_int32> min (0, 0, 0), max (obj->length(), obj->width(), obj->height());
    world::chunk_info ci (&e, min, max);
    
    // get object extend
    int x, y, l, h;
    getObjectExtend (&ci, x, y, l, h);
    
    // create a surface onto which to draw the picked object
    DrawObjSurface = gfx::create_surface ();
    DrawObjSurface->set_alpha (128, true);
    DrawObjSurface->resize (l, h);
    DrawObjSurface->fillrect (0, 0, l, h, 0x00000000);
    
    // properly render the object
    std::list <world::chunk_info*> object_list;
    object_list.push_back (&ci);
    gfx::drawing_area da (0, 0, l, h);
    world::debug_renderer renderer (true);
    renderer.render (-x, -y, object_list, da, DrawObjSurface);
    
    // give tile some contrast
    DrawObjSurface->set_brightness (150);
    
    // scale
    if (base::Scale > 1)
    {
        gfx::surface *tmp = gfx::create_surface ();
        tmp->set_alpha (128, true);
        tmp->resize (l*base::Scale, h*base::Scale);

        DrawObjSurface->scale_up(tmp, base::Scale);
        delete DrawObjSurface;
        DrawObjSurface = tmp;
    }

    if (CandidateObj == NULL && CurObj == ety)
    {
        // entity picked from the map? --> use as new reference for grid
        Grid->set_reference (CurObj->getLocation()->center_min(), CurObj);
    }

    // update grid
    Grid->grid_from_object (ety, area->x(), area->y());
    Grid->set_visible (true);
    Grid->draw ();
    
    // update zones, if displayed
    Zones->update();

    // update overlap indication
    highlightObject();

    // update screen
    GdkRectangle rect = { 0, 0, Overlay->length(), Overlay->height() };
    gdk_window_invalidate_rect (gtk_widget_get_window (Screen), &rect, FALSE);
}

// drop object currently picked for drawing
void GuiMapview::releaseObject ()
{
    if (DrawObj != NULL)
    {
        // stop selection, if it is in progress
        CandidateObj = NULL;

        // hide grid
        Grid->set_visible (false);
        Grid->draw ();
        
        // update zones, if displayed
        Zones->update();

        // update screen
        GdkRectangle rect = { 0, 0, Overlay->length(), Overlay->height() };
        gdk_window_invalidate_rect (gtk_widget_get_window (Screen), &rect, FALSE);
        
        // clear selection, so we can select it again
        GuiMapedit::window->entityList()->setSelected (DrawObj, false);

        // cleanup
        delete DrawObjSurface;
        DrawObjSurface = NULL;
        DrawObj = NULL;
        
        // highlight new object
        highlightObject();

        // update entity list, if filtered by connectors
        if (GuiFilterDialog::getActiveFilter() == GuiFilterDialog::BY_CONNECTOR)
        {
            GuiMapedit::window->entityList()->filterChanged();
        }
    }
}

// move an object already present on map
void GuiMapview::moveCurObject()
{
    if (DrawObj == NULL && CurObj != NULL)
    {
        // keep track of location
        world::chunk_info *location = CurObj->getLocation();
     
        // select object for drawing ...
        selectCurObj();

        // ... and remove it from map
        if (CurObj->removeAtCurLocation())
        {
            // on success, redraw area containing object
            Renderer.clearSelection();
            renderObject(location);
        }
        
        // show drawing object
        highlightObject();
    }
}

// add object to map
void GuiMapview::placeCurObj()
{
    if (CandidateObj != NULL)
    {
        // selection not finished, prevent placing
        return;
    }

    if (isScrolling())
    {
        // FIXME
        // placing objects while scrolling currently
        // causes them to be out-of-sync with the grid
        return;
    }
    
    if (DrawObj != NULL && (DrawObj->canPlaceOnMap() || !Grid->overlap_prevented()))
    {
        MapData *area = (MapData*) MapMgr::get_map();
        world::entity *ety = DrawObj->entity();
        
        // check if object can be placed safely
        if (ety->has_name() && DrawObj->getRefCount() > 0)
        {
            MapEntity *newObj = new MapEntity (ety);
            
            // need to make a copy with new id first
            GuiEntityDialog dlg (newObj, GuiEntityDialog::DUPLICATE_NAMED_ENTITY);
            if (!dlg.run())
            {
                // user canceled and object has not been added to map
                delete newObj;
                return;
            }

            // update meta data
            newObj->loadMetaData();

            // update draw object
            DrawObj = newObj;
            ety = newObj->entity();

            // add new entity to entity list
            GuiMapedit::window->entityList()->addEntity (newObj);
        }

        // get object height
        int h = ety->get_object()->cur_z() + ety->get_object()->height() - ety->get_object()->cur_y();
                
        // get proper coordinates
        world::coordinates pos (DrawObjPos.x() + area->x(), DrawObjPos.y() + area->y() + h, area->z()); 
        
        // try adding object to map
        if (DrawObj->addToLocation (pos))
        {
            // cannot place same object twice at this position
            indicateOverlap();
            
            // update grid, so that auto-adjust will deliver the correct offset
            Grid->set_reference (pos, DrawObj);
            
            // update screen
            render (DrawObjPos.x(), DrawObjPos.y(), DrawObjSurface->length() / base::Scale, DrawObjSurface->height() / base::Scale);
            
            // update valid height range
            RenderHeight->setMapExtend(area->min().z(), area->max().z());
        }
    }
}

// edit highlighted object's properties
void GuiMapview::editCurObject ()
{
    if (DrawObj == NULL && CurObj != NULL)
    {
        GuiEntityDialog dlg (CurObj, GuiEntityDialog::UPDATE_PROPERTIES);
        if (dlg.run())
        {
            // object state could have changed --> redraw
            if (CurObj->getRefCount() > 1)
            {
                // object can be anywhere on the map --> redraw everything
                render ();
            }
            else
            {
                // object exists only once, so redraw only object
                renderObject (CurObj->getLocation());
            }
        }
    }
}

// delete object from map
void GuiMapview::deleteCurObj ()
{
    if (DrawObj == NULL && CurObj != NULL)
    {
        // keep track of location
        world::chunk_info *location = CurObj->getLocation();
        
        // remove object from map
        if (CurObj->removeAtCurLocation())
        {
            // on success, redraw area containing object
            Renderer.clearSelection();
            renderObject (location);

            // clear selection, so we can select it again
            GuiMapedit::window->entityList()->setSelected (CurObj, false);
            CurObj = NULL;
            
            // see if there's another object to select
            highlightObject();
        }
    }
}

void GuiMapview::showZones (const bool & show)
{
    Zones->set_visible (show);
    Zones->draw(0, 0, Overlay->length(), Overlay->height());
    
    // redraw grid, if visible
    Grid->draw();

    // redraw
    draw();
}

// prepare everything for 'auto-scrolling' (TM) ;-)
bool GuiMapview::scrollingAllowed () const
{
    // is there a reason to scroll at all?
    MapData *area = (MapData*) MapMgr::get_map();
    return area != NULL;
}

// the actual scrolling
void GuiMapview::scroll ()
{
    // update area coordinates
    MapData *area = (MapData*) MapMgr::get_map();
    area->setX (area->x() - scroll_offset.x);
    area->setY (area->y() - scroll_offset.y);

    // update grid
    Grid->scroll (scroll_offset.x, scroll_offset.y);
    
    // update zones, if displayed
    Zones->update();

    // rendering the whole mapview is less performant than doing
    // gdk_window_scroll (GDK_WINDOW(gtk_widget_get_window (Screen)), scroll_offset.x, scroll_offset.y);
    // but it appears to be the only way to prevent artifacts from appearing
    
    // render at new position
    render ();
    
    // update map coordinates of mouse pointer
    updateLocation (area);
}

// set a new position
void GuiMapview::gotoPosition (const s_int32 & x, const s_int32 & y, const s_int32 & z)
{
    // we want to display the center position
    GtkAllocation allocation;
    gtk_widget_get_allocation (Screen, &allocation);
    int ox = allocation.width / (2 * base::Scale);
    int oy = allocation.height / (2 * base::Scale);

    // update z coordinate
    MapData *area = (MapData*) MapMgr::get_map();
    area->setZ(z);

    // get x and y offset
    scroll_offset.x = area->x() - x + ox;
    scroll_offset.y = area->y() - y + oy;

    // scroll into view
    scroll ();
}

// highlight object under mouse, if any
void GuiMapview::highlightObject ()
{
    gint x, y;
    gtk_widget_get_pointer (Screen, &x, &y);
    GdkPoint mouseLoc = { x, y };
    mouseMoved (&mouseLoc);
}

// calculate rendered object's size
void GuiMapview::getObjectExtend (world::chunk_info *obj, int & x, int & y, int & l, int & h)
{
    // get location
    x = obj->Min.x();
    y = obj->Min.y() - obj->Max.z();
    
    // get extend
    l = obj->Max.x() - x;
    h = obj->Max.y() - obj->Min.z() - y;
}
