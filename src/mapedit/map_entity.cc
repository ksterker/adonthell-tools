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
 * @file mapedit/map_entity.cc
 *
 * @author Kai Sterker
 * @brief Meta data for entities used on the map.
 */

#include <gtk/gtk.h>
#include <world/renderer.h>

#include "backend/gtk/screen_gtk.h"
#include "mapedit/map_entity.h"

// ctor
MapEntity::MapEntity (world::entity *obj)
{
    Entity = obj;
    
    update_tags ();
}

// update tags
void MapEntity::update_tags ()
{
    Tags.clear ();

    world::placeable *obj = Entity->get_object();
    std::string path = obj->filename();
    gchar *dir_name = g_path_get_dirname (path.c_str());
    gchar **tags = g_strsplit (dir_name, "/", -1);

    for (gchar **iter = tags; *iter != NULL; *iter++)
    {
        Tags.push_back (*iter);
    }

    g_strfreev (tags);
}

// name (and id) of entity
gchar* MapEntity::get_name_and_id () const
{
    world::placeable *obj = Entity->get_object();
    std::string path = obj->filename();
    gchar* name = g_path_get_basename (path.substr (0, path.rfind(".")).c_str());
    
    if (Entity->has_name ())
    {
        gchar *tmp = g_strconcat (name, "\n", Entity->id()->c_str(), NULL);
        g_free (name);
        name = tmp;
    }        
    
    return name;
}

// type of entity: (A)nonymous, (S)hared or (U)nique
gchar* MapEntity::get_type_name () const
{
    return g_strconcat ((Entity->has_name() ? (((world::named_entity*)Entity)->is_unique() ? "U" : "S") : "A"), NULL);
}

// picture of entity
GdkPixbuf *MapEntity::get_icon () const
{
    static world::default_renderer renderer;
    world::placeable *obj = Entity->get_object();

    // pixmap extends
    int l = obj->length();
    int h = obj->width() + obj->height();
    
    // create pixmap
    GdkPixmap* pixmap = gdk_pixmap_new (NULL, l, h, gdk_visual_get_best_depth ());
    
    // render entity onto a pixmap
    gfx::screen_surface_gtk *surface = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
    surface->set_drawable (pixmap);
    surface->set_alpha (255, true);
    surface->resize (l, h);
    surface->fillrect (0, 0, l, h, 0xFFFFFFFF);
    
    // properly render the object
    world::vector3<s_int32> min (0, 0, 0), max (obj->length(), obj->width(), obj->height()); 
    world::chunk_info ci (Entity, min, max);
    std::list <world::chunk_info*> object_list;
    object_list.push_back (&ci);
    gfx::drawing_area da (0, 0, l, h);
    renderer.render (0, obj->height(), object_list, da, surface);
    
    // thumbnail of entity
    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, gdk_colormap_get_system (), 0, 0, 0, 0, l, h);
    int nl = l > h ? 32 : ((float) l / h) * 32;
    int nh = h > l ? 32 : ((float) h / l) * 32;
    GdkPixbuf *icon = gdk_pixbuf_scale_simple (pixbuf, nl, nh, GDK_INTERP_BILINEAR);
    
    // cleanup
    g_object_unref (pixmap);
    g_object_unref (pixbuf);
    
    return icon;
}
