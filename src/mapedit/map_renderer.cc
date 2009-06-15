/*
 *  map_renderer.cc
 *  Adonthell-Tools
 *
 *  Created by Kai Sterker on 31.03.09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include <gdk/gdk.h>
#include <gfx/gfx.h>

#include "map_renderer.h"

static world::render_info * curObj = NULL;

// ctor
MapRenderer::MapRenderer () : default_renderer ()
{
    SelectedObject = NULL;
    FindObjectBelowCursor = false;
}

// dtor
MapRenderer::~MapRenderer ()
{
}

// find the object at current mouse position
world::chunk_info* MapRenderer::findObjectBelowCursor (const s_int32 & ox, const s_int32 & oy, const GdkPoint *mousePos, const std::list <world::chunk_info*> & objectlist)
{
    // not much to do if there is only one object in the list
    if (objectlist.size () == 1)
    {
        SelectedObject = objectlist.front();
        return SelectedObject;
    }
    
    static gfx::drawing_area da;
    MousePos = mousePos;
    FindObjectBelowCursor = true;
    
    // this will identify object under cursor, it won't actually draw anything
    render (ox, oy, objectlist, da, NULL);
    
    // find object in list
    SelectedObject = NULL;
    if (curObj != NULL)
    {
        for (std::list <world::chunk_info*>::const_iterator i = objectlist.begin(); i != objectlist.end(); i++)
        {
            if (belongsToObject (*i, curObj))
            {
                SelectedObject = *i;
                break;
            }
        }
    }
    
    // reset for normal drawing
    FindObjectBelowCursor = false;
    
    return SelectedObject;
}

void MapRenderer::draw (const s_int16 & x, const s_int16 & y, const world::render_info & obj, const gfx::drawing_area & da, gfx::surface * target) const
{
    if (FindObjectBelowCursor)
    {
        // get area covered by object
        GdkRectangle rect = {
            obj.x (),
            obj.Pos.y () - obj.z() - obj.Shape->height(),
            obj.Sprite->length(),
            obj.Sprite->height()
        };
        
        // mouse is not in area covered by object
        if (MousePos->x < rect.x || MousePos->x > rect.x + rect.width ||
            MousePos->y < rect.y || MousePos->y > rect.y + rect.height)
        {
            return;
        }
        
        // set new object
        delete curObj;
        curObj = new world::render_info (obj);
    }
    else
    {
        // highlight selected sprite
        if (SelectedObject != NULL && belongsToObject (SelectedObject, &obj))
        {
            gfx::surface *highlight = gfx::create_surface();
            highlight->set_alpha (255, true);
            highlight->resize (obj.Sprite->length(), obj.Sprite->height());
            obj.Sprite->draw (0, 0, NULL, highlight);
            highlight->set_brightness (150);
            
            // draw frame around selected object
            u_int32 col = highlight->map_color (255, 255, 255);
            
            highlight->draw_line (0, 0, highlight->length() - 1, 0, col);
            highlight->draw_line (0, 0, 0, highlight->height() - 1, col);
            highlight->draw_line (highlight->length() - 1, 0, highlight->length() - 1, highlight->height() - 1, col);
            highlight->draw_line (0, highlight->height() - 1, highlight->length() - 1, highlight->height() - 1, col);
            
            highlight->draw (x + obj.x () + obj.Shape->ox(), y + obj.y () + obj.Shape->oy() - obj.z() - obj.Shape->height(), &da, target);
            delete highlight;
        }
        else
        {
            // render object unchanged
            obj.Sprite->draw (x + obj.x () + obj.Shape->ox(), y + obj.y () + obj.Shape->oy() - obj.z() - obj.Shape->height(), &da, target);
        }
    }
}

// check if the render_info is a part of the chunk_info
bool MapRenderer::belongsToObject (const world::chunk_info *ci, const world::render_info *ri) const
{
    const world::placeable *object = ci->get_object();
    
    for (world::placeable::iterator obj = object->begin(); obj != object->end(); obj++)
    {
        if ((*obj)->get_sprite() == ri->Sprite && ci->Min == ri->Pos)
        {
            return true;
        }
    }    
    
    return false;
}
