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
 * @file mapedit/gui_entity_list.cc
 *
 * @author Kai Sterker
 * @brief Display entities used on the map.
 */

#include <gtk/gtk.h>
#include <world/renderer.h>

#include "backend/gtk/screen_gtk.h"
#include "mapedit/gui_entity_list.h"

enum
{
    NAME_COLUMN,
    TYPE_COLUMN,
    ICON_COLUMN,
    NUM_COLUMNS
};

// ctor
GuiEntityList::GuiEntityList ()
{
    // the view
    TreeView = (GtkTreeView*) gtk_tree_view_new ();
    gtk_tree_view_set_grid_lines (TreeView, GTK_TREE_VIEW_GRID_LINES_HORIZONTAL);
    
    // create the columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new ();
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Name", renderer, "text", NAME_COLUMN, NULL);
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Type", renderer, "text", TYPE_COLUMN, NULL);
    renderer = gtk_cell_renderer_pixbuf_new ();
    gtk_tree_view_insert_column_with_attributes (TreeView, -1, "Icon", renderer, "pixbuf", ICON_COLUMN, NULL);

    // create the (empty) model
    GtkListStore *model = gtk_list_store_new (NUM_COLUMNS, G_TYPE_STRING, G_TYPE_STRING, GDK_TYPE_PIXBUF);
    gtk_tree_view_set_model (TreeView, (GtkTreeModel*) model);
}

void GuiEntityList::setMap (MapData * map)
{
    // TODO: cleanup previously set data
    
    Map = map;
    GtkTreeIter iter;
    world::default_renderer renderer;
    
    // get model
    GtkListStore *model = (GtkListStore *) gtk_tree_view_get_model (TreeView);
    
    // fill model
    for (MapData::entity_iter e = map->firstEntity(); e != map->lastEntity(); e++)
    {
        // get new row
        gtk_list_store_append (model, &iter);
        
        // name (and id) of entity
        world::placeable *obj = (*e)->get_object();
        std::string path = obj->filename();
        gchar* name = g_path_get_basename (path.substr (0, path.rfind(".")).c_str());
        
        if ((*e)->has_name ())
        {
            gchar *tmp = g_strconcat (name, "\n", (*e)->id()->c_str(), NULL);
            g_free (name);
            name = tmp;
        }        
        
        // type of entity: (A)nonymous, (S)hared or (U)nique
        gchar *type = (gchar*) ((*e)->has_name() ? (((world::named_entity*)(*e))->is_unique() ? "U" : "S") : "A");

        // picture of entity
        int l = obj->length();
        int h = obj->width() + obj->height();
        
        // create pixmap
        GdkPixmap* pixmap = gdk_pixmap_new (NULL, l, h, gdk_visual_get_best_depth ());
        
        // render entity onto a pixmap
        gfx::screen_surface_gtk *s = (gfx::screen_surface_gtk*) gfx::screen::get_surface();
        s->set_drawable (pixmap);
        s->set_alpha (255, true);
        s->resize (l, h);
        s->fillrect (0, 0, l, h, 0xFFFFFFFF);
        
        // properly render the object
        world::vector3<s_int32> min (0, 0, 0), max (obj->length(), obj->width(), obj->height()); 
        world::chunk_info ci (*e, min, max);
        std::list <world::chunk_info*> object_list;
        object_list.push_back (&ci);
        gfx::drawing_area da (0, 0, l, h);
        renderer.render (0, obj->height(), object_list, da, s);

        // thumbnail of entity
        GdkPixbuf *pixbuf = gdk_pixbuf_get_from_drawable (NULL, pixmap, gdk_colormap_get_system (), 0, 0, 0, 0, l, h);
        int nl = l > h ? 32 : ((float) l / h) * 32;
        int nh = h > l ? 32 : ((float) h / l) * 32;
        GdkPixbuf *icon = gdk_pixbuf_scale_simple (pixbuf, nl, nh, GDK_INTERP_BILINEAR);
        
        gtk_list_store_set (model, &iter, NAME_COLUMN, name, TYPE_COLUMN, type, ICON_COLUMN, icon, -1);
        
        g_object_unref (pixmap);
        g_object_unref (pixbuf);
        // g_object_unref (type);
        // g_object_unref (name);
    }
}
