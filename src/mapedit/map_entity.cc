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

#include <algorithm>
#include <cctype>
#include <gtk/gtk.h>

#include <world/renderer.h>
#include <world/object.h>
#include <world/character.h>

#include "backend/gtk/screen_gtk.h"
#include "gui_filter_dialog.h"
#include "map_entity.h"
#include "map_data.h"

// ctor
MapEntity::MapEntity (world::entity *obj, const u_int32 & count)
{
    Location = NULL;
    Entity = obj;
    Object = obj->get_object ();
    RefCount = count;
}

// ctor
MapEntity::MapEntity (world::placeable *obj)
{
    Location = NULL;
    Entity = NULL;
    Object = obj;
    RefCount = 0;
}

// dtor
MapEntity::~MapEntity()
{
    remove_tags ();
}

void MapEntity::loadMetaData ()
{
    update_tags();

    std::string name = Object->modelfile();
    size_t idx = name.find_last_of('.');
    if (idx == std::string::npos)
    {
        return;
    }

    std::string meta_file_name = name.substr(0, idx) + ".xtra";
    if (!base::Paths().find_in_path(meta_file_name, false))
    {
        return;
    }

    base::diskio meta_data (base::diskio::BY_EXTENSION);
    if (!meta_data.get_record (meta_file_name))
    {
        return;
    }

    // load description
    Comment = meta_data.get_string("dscr");

    // load tags
    void *value;
    base::flat tags = meta_data.get_flat("tags");
    while (tags.next(&value) != base::flat::T_UNKNOWN)
    {
        add_tag(strdup ((gchar *) value));
    }

    // load connectors
    u_int32 size;
    base::flat connectors = meta_data.get_flat("ctrs");
    while (connectors.next(&value, &size) != base::flat::T_UNKNOWN)
    {
        base::flat connector ((char *) value, size);
        MdlConnectorTemplate *tmpl = MdlConnectorManager::get(connector.get_uint32("tmpl"));
        if (tmpl != NULL)
        {
            MdlConnector *ctor = new MdlConnector (tmpl);
            ctor->set_side((MdlConnector::face) connector.get_uint8 ("side"));
            ctor->set_pos(connector.get_sint16 ("pos"));

            Connectors.push_back(ctor);
        }
    }
}

// create or update entity
bool MapEntity::update_entity (const world::placeable_type & obj_type, const char & entity_type, const std::string & id)
{
    // get map associated with the object
    MapData *map = (MapData*) &(Object->map());    

    // do we have a shared entity? Object of different type?
    if (entity_type != 'S' && obj_type != Object->type())
    {
        world::placeable *obj = NULL;
        
        // create object
        switch (obj_type)
        {
            case world::OBJECT:
            {
                obj = new world::object (*map);
                break;
            }
            case world::CHARACTER:
            {
                obj = new world::character (*map);
                break;
            }
            default:
            {
                fprintf (stderr, "*** area::get_state: unknown object type %i\n", obj_type);
                return false;
            }
        }
        
        // load object data ...
        obj->load_model (Object->modelfile());

        // ... and set state
        obj->set_state (Object->state());
        
        // cleanup (only if object not yet on the map)
        if (Entity == NULL)
        {
            delete Object;
        }
        
        Object = obj;
    }    

    // create entity
    switch (entity_type)
    {
        case 'A':
        {
            Entity = new world::entity(Object);
            break;
        }
        case 'U':
        {
            Entity = new world::named_entity(Object, id, true);
            break;
        }
        case 'S':
        {
            Entity = new world::named_entity(Object, id, false);
            break;
        }
        default:
        {
            fprintf (stderr, "update_entity: unsupported entity type '%c'.\n", entity_type);
            return false;
        }
    }
    
    return true;
}

// rename map entity
bool MapEntity::rename (const std::string & id)
{
    // cannot rename in that case
    if (Entity == NULL || Entity->id () == NULL) return false;
    
    // no need to rename
    if (*Entity->id() == id) return true;
    
    // get map associated with the object
    MapData *map = (MapData*) &(Object->map());

    // rename object on map
    return map->renameEntity (this, id) != NULL;
}

// decrease reference count
void MapEntity::decRef ()
{
    RefCount--; 
    
    // it's no longer present on map
    Location = NULL;
    
    // remove entity from map structure
    if (RefCount == 0)
    {
        MapData *map = (MapData*) &(Object->map());
        map->remove_entity (this);
    
        // TODO: fireEntityListChanged
    }
}

// increase reference count
void MapEntity::incRef ()
{
    RefCount++;
    
    if (RefCount == 1)
    {
        // add new entity to map
        MapData *map = (MapData*) &(Object->map());
        map->add_entity (Entity);
    }
}

// place this entity on the map
bool MapEntity::addToLocation(const world::coordinates & pos)
{
    // get map associated with the object
    MapData *map = (MapData*) &(Object->map());
    
    // make sure we don't place same object twice at same location
    if (!((world::area*)map)->exists (Entity, pos))
    {
        // place object on map
        map->add (Entity, pos);
        
        // update refcount
        incRef();
        
        // update moveable position
        world::moving *mov = dynamic_cast<world::moving*>(Object);
        if (mov != NULL)
        {
            mov->set_position (pos.x(), pos.y());
            mov->set_altitude (pos.z());
        }

        return true;
    }
    
    return false;
}

// remove this entity from the map
bool MapEntity::removeAtCurLocation()          
{
    if (Location != NULL)
    {
        // get map associated with the object
        MapData *map = (MapData*) &(Object->map());
        if (map->remove (*Location) != NULL)
        {
            decRef();
            return true;
        }
    }
    
    return false;
}

// check intersection of map entity at given position with objects from the given list
bool MapEntity::intersects (const std::list<world::chunk_info*> & objects, const world::vector3<s_int32> & pos)
{
    // check all objects returned
    for (std::list<world::chunk_info*>::const_iterator i = objects.begin(); i != objects.end(); i++)
    {
        const world::placeable *object = (*i)->get_object();

        // check all models the placeable consists of
        for (world::placeable::iterator model = object->begin(); model != object->end(); model++)
        {
            // get the model's current shape, ...
            const world::placeable_shape *shape = (*model)->current_shape ();
            if (shape->is_solid() && intersects (shape, pos - (*i)->center_min()))
            {
                Overlapping = true;
                return true;
            }
        }
    }

    Overlapping = false;
    return false;
}

// check for intersection of a specific shape with this object
bool MapEntity::intersects (const world::placeable_shape *other_shape, const world::vector3<s_int32> & offset) const
{
    // check all models this entity consists of
    for (world::placeable::iterator model = Object->begin(); model != Object->end(); model++)
    {
        const world::placeable_shape *shape = (*model)->current_shape ();
        if (shape->is_solid() && shape->intersects (other_shape, offset))
        {
            return true;
        }
    }

    return false;
}

void MapEntity::add_tag(const gchar *tag)
{
    // TODO: check for duplicate tag

    GtkTreeIter row;
    GtkListStore *filter_model = GuiFilterDialog::getFilterModel();

    if (GuiFilterDialog::findTagInFilter(tag, &row))
    {
        // tag already present? -> update count
        guint count;
        gtk_tree_model_get (GTK_TREE_MODEL(filter_model), &row, 2, &count, -1);
        gtk_list_store_set (filter_model, &row, 2, count+1, -1);
    }
    else
    {
        // otherwise insert new row
        bool enabled = strcmp(tag, "template") != 0;

        gtk_list_store_append (filter_model, &row);
        gtk_list_store_set (filter_model, &row, 0, enabled, 1, tag, 2, 1, -1);
    }

    Tags.push_back(tag);
}

// update tags
void MapEntity::update_tags ()
{
    Tags.clear ();

    std::string path = Object->modelfile();
    gchar *dir_name = g_path_get_dirname (path.c_str());
    gchar **tags = g_strsplit (dir_name, "/", -1);

    for (gchar **iter = tags; *iter != NULL; iter++)
    {
        add_tag(*iter);
    }

    g_strfreev (tags);
    g_free (dir_name);
}

// remove all tags this entity contained
void MapEntity::remove_tags()
{
    for (std::vector<std::string>::const_iterator i = Tags.begin(); i != Tags.end(); i++)
    {
        GtkTreeIter row;
        GtkListStore *filter_model = GuiFilterDialog::getFilterModel();

        if (GuiFilterDialog::findTagInFilter(i->c_str(), &row))
        {
            // decrease tag count
            guint count;
            gtk_tree_model_get (GTK_TREE_MODEL(filter_model), &row, 2, &count, -1);
            gtk_list_store_set (filter_model, &row, 2, count-1, -1);
        }
    }
}

// name of entity
gchar* MapEntity::get_name () const
{
    std::string path = Object->modelfile();
    gchar* name = g_path_get_basename (path.substr (0, path.rfind(".")).c_str());
    return name;
}

gchar* MapEntity::createNewId () const
{
    std::string base;
    if (Entity && Entity->has_name ())
    {
        base = *Entity->id();
    }
    else
    {
        gchar *file_name = get_name();
        base = std::string (file_name);
        g_free (file_name);
    }
 
    // strip any trailing number from id
    size_t pos = base.find_last_of ("-_");
    if (pos != base.npos)
    {
        bool isnum = true;
        for (size_t i = pos + 1; i < base.length(); i++)
        {
            isnum &= isdigit (base[i]);
        }
        if (isnum)
        {
            base = base.substr(0, pos);
        }
    }
    
    // get map associated with the object
    MapData *map = (MapData*) &(Object->map());
    
    // try to create unique id
    u_int32 i = 0;
    std::stringstream id (std::ios::out);
    do
    {
        id.str ("");
        id << base << "_" << ++i;
    }
    while (map->exists (id.str()));

    // return this id as newly allocated string
    return g_strdup_printf ("%s_%i", base.c_str(), i);
}

// id of entity
gchar* MapEntity::get_id () const
{
    gchar *id = NULL;
    if (Entity && Entity->has_name ())
    {
        id = g_strdup (Entity->id()->c_str());
    }
    return id;
}

// type of entity: (A)nonymous, (S)hared or (U)nique
gchar* MapEntity::get_entity_type () const
{
    if (Entity)
    {
        return g_strdup ((Entity->has_name() ? (((world::named_entity*)Entity)->is_unique() ? "U" : "S") : "A"));
    }
    
    return g_strdup ("-");
}

// picture of entity
GdkPixbuf *MapEntity::get_icon (const u_int32 & size) const
{
    static world::default_renderer renderer;

    // pixmap extends
    int l = Object->length();
    int h = Object->width() + Object->height();
    
    // create pixmap
    GdkVisual *vis = gdk_visual_get_system();
    GdkPixmap* pixmap = gdk_pixmap_new (NULL, l, h, gdk_visual_get_depth (vis));
    gdk_drawable_set_colormap (GDK_DRAWABLE(pixmap), gdk_colormap_get_system ());
    
    // render entity onto a pixmap
    gfx::screen_surface_gtk *surface = (gfx::screen_surface_gtk*) gfx::screen::get_surface();

    // determine background color
    u_int32 color = 0xFFFFFFFF;
    if (isOnMap())
    {
        // present on map, but unused
        if (RefCount == 0)
        {
            color = surface->map_color (0x80, 0xC8, 0xFE, 0xFF);
        }
    }
    else
    {
        // not present on map yet
        color = surface->map_color (0xFE, 0xB3, 0x80, 0xFF);
    }
    
    surface->set_drawable (pixmap);
    surface->set_alpha (255, true);
    surface->resize (l, h);
    surface->fillrect (0, 0, l, h, color);
    
    // properly render the object
    world::vector3<s_int32> min (0, 0, 0), max (Object->length(), Object->width(), Object->height());
    world::named_entity ety (Object, "", false);
    world::chunk_info ci (&ety, min, max);
    std::list <world::chunk_info*> object_list;
    object_list.push_back (&ci);
    gfx::drawing_area da (0, 0, l, h);
    renderer.render (0, Object->height(), object_list, da, surface);
    
    // thumbnail of entity
    GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, gdk_colormap_get_system (), 0, 0, 0, 0, l, h);
    int nl = l > h ? size : ((float) l / h) * size + 1;
    int nh = h > l ? size : ((float) h / l) * size + 1;
    GdkPixbuf *icon = gdk_pixbuf_scale_simple (pixbuf, nl, nh, GDK_INTERP_BILINEAR);
    
    // cleanup
    g_object_unref (pixmap);
    g_object_unref (pixbuf);
    
    return icon;
}

// return states of object
std::hash_set<std::string> MapEntity::get_object_states () const
{
    static std::hash_set<std::string> states;
    
    states.clear();
    for (world::placeable::iterator i = Object->begin(); i != Object->end(); i++)
    {
        for (world::placeable_model::iterator j = (*i)->begin(); j != (*i)->end(); j++)
        {
            states.insert (j->first);
        }
    }
    
    return states;
}

// return type of placeable
world::placeable_type MapEntity::get_object_type () const
{
    if (Entity)
    {
        // we know the type
        return Entity->get_object()->type();
    }
    
    // we don't know the type yet, but we can make an educated guess
    if (std::find (Tags.begin(), Tags.end(), std::string ("char")) != Tags.end())
    {
        return world::CHARACTER;
    }
    if (std::find (Tags.begin(), Tags.end(), std::string ("item")) != Tags.end())
    {
        return world::ITEM;
    }
    
    return world::OBJECT;
}
