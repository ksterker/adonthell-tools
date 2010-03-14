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

#include "map_data.h"
#include "map_entity.h"

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
 * A list of all entities either present on the map or available
 * in the gamedata's model directory. It allows to add latter to
 * the map and to pick former for placement on the map.
 */
class GuiEntityList
{
public:
    /**
     * Create the user interface elements.
     */
    GuiEntityList ();
    
    /**
     * Select the given entity in the entity list.
     * @param etyToSelect the entity to select.
     * @param select true to select object, false to clear selection.
     * @return true if found, false otherwise.
     */
    bool setSelected (MapEntity *etyToSelect, const bool & select = true);        

    /**
     * Find the given entity in the entity list.
     * @param etyToFind an entity present on the map.
     * @return the editor's wrapper around the entity or NULL.
     */
    MapEntity *findEntity (const world::entity *etyToFind) const;
    
    /**
     * Add a new entity to the entity list.
     * @param ety the entity to add to the list.
     */
    void addEntity (MapEntity *ety);
    
    /**
     * Set the map whose entities to display in the list.
     * @param map the map whose entities to display.
     */
    void setMap (MapData * map);
    
    /**
     * Set the "model" directory. It is scanned for map objects
     * not yet present on the map. They will be added to the
     * entity list and can be placed on the map.
     * @param datadir the directory to scan recursively.
     */
    void setDataDir (const std::string & datadir);

    /**
     * Refresh the list of available map objects if things 
     * have been changed on the file system.
     */
    void refresh();
    
    /**
     * Check whether the given entity is already present on the
     * map. To achieve this, filenames are compared.
     * @param filename the name of the entity to check.
     * @return true if entity is part of the map, false otherwise.
     */
    bool isPresentOnMap (const std::string & filename) const;
    
    /**
     * Get the tree view widget.
     * @return pointer to the tree view.
     */
    GtkWidget *getWidget () const { return Panel; }

protected:
    /**
     * Scan the given directory for models and add them to the
     * given list store.
     * @param datadir the directory to scan recursively.
     * @param model the list to add an models.
     */
    void scanDir (const std::string & datadir, GtkListStore *model);
    
private:
    /// the data directory containing entities
    std::string DataDir; 
    /// the map whose entities are displayed
    MapData* Map;
    /// the list view
    GtkTreeView *TreeView;
    /// the whole zone panel
    GtkWidget *Panel;
};

#endif
