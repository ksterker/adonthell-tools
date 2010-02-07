/* 
 Copyright (C) 2010 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file mapedit/gui_zone_list.h
 *
 * @author Kai Sterker
 * @brief Display zones used on the map.
 */

#ifndef GUI_ZONE_LIST_H
#define GUI_ZONE_LIST_H

#include <gtk/gtk.h>

#include "map_data.h"
#include "map_entity.h"

G_BEGIN_DECLS

#define TYPE_ZONE_LIST	(zone_list_get_type ())
#define ZONE_LIST(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_ZONE_LIST, ZoneList))
#define ZONE_LIST_CLASS(obj)	(G_TYPE_CHECK_CLASS_CAST ((obj), TYPE_ZONE_LIST, ZoneListClass))
#define IS_ZONE_LIST(obj)	(G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_ZONE_LIST))
#define IS_ZONE_LIST_CLASS(obj)	(G_TYPE_CHECK_CLASS_TYPE ((obj), TYPE_ZONE_LIST))
#define ZONE_LIST_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_ZONE_LIST, ZoneListClass))

typedef struct _ZoneList ZoneList;
typedef struct _ZoneListClass ZoneListClass;

struct _ZoneList
{
	GtkListStore parent;
};

struct _ZoneListClass
{
	GtkListStoreClass parent_class;
};

GType zone_list_get_type (void);

G_END_DECLS

/**
 * A list of all zones present on the map.
 */
class GuiZoneList
{
public:
    /**
     * Create the user interface elements.
     */
    GuiZoneList ();
    
    /**
     * Create a new zone.
     */
    void addZone();
    
    /**
     * Delete an existing zone.
     */
    void removeZone();
    
    /**
     * Set the map whose zones to display in the list.
     * @param map the map whose zones to display.
     */
    void setMap (MapData * map);
    
    /**
     * Get the zone view widget.
     * @return pointer to the zone view.
     */
    GtkWidget *getWidget () const { return (GtkWidget*) Panel; }
    
private:
    /// the map whose entities are displayed
    MapData* Map;
    /// the list view
    GtkTreeView *TreeView;
    /// the whole zone panel
    GtkWidget *Panel;
};

#endif
