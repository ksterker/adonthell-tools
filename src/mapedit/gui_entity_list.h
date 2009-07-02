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
 * @file mapedit/gui_entity_list.h
 *
 * @author Kai Sterker
 * @brief Display entities used on the map.
 */

#ifndef GUI_ENTITY_LIST_H
#define GUI_ENTITY_LIST_H

#include <gtk/gtk.h>

#include "mapedit/map_data.h"
#include "mapedit/map_entity.h"

G_BEGIN_DECLS

#define TYPE_ENTITY_LIST	(entity_list_get_type ())
#define ENTITY_LIST(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ENTITY_LIST, EntityList))
#define ENTITY_LIST_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), TYPE_ENTITY_LIST, EntityListClass))
#define IS_ENTITY_LIST(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ENTITY_LIST))
#define IS_ENTITY_LIST_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), TYPE_ENTITY_LIST))
#define ENTITY_LIST_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ENTITY_LIST, EntityListClass))

typedef struct _EntityList EntityList;
typedef struct _EntityListClass EntityListClass;

struct _EntityList
{
	GtkListStore parent;
};

struct _EntityListClass
{
	GtkListStoreClass parent_class;
};

GType entity_list_get_type (void);

G_END_DECLS

/**
 *
 *
 */
class GuiEntityList
{
public:
    /**
     * Create the user interface elements.
     */
    GuiEntityList ();
    
    /**
     * Set the map whose entities to display in the list.
     * @param map the map whose entities to display.
     */
    void setMap (MapData * map);
    
    /**
     * Get the tree view widget.
     * @return pointer to the tree view.
     */
    GtkWidget *getTreeWidget () const { return (GtkWidget*) TreeView; }
    
private:
    /// the map whose entities are displayed
    MapData* Map;
    /// the list view
    GtkTreeView *TreeView;
};

#endif
