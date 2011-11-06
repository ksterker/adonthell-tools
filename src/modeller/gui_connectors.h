/*
 Copyright (C) 2011 Kai Sterker <kaisterker@linuxgames.com>
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
 * @file modeller/gui_connectors.h
 *
 * @author Kai Sterker
 * @brief Dialog for editing connector templates.
 */

#ifndef GUI_CONNECTORS_H_
#define GUI_CONNECTORS_H_

#include <gtk/gtk.h>
#include "mdl_connector.h"
#include "gui_modal_dialog.h"

G_BEGIN_DECLS

#define TYPE_CONNECTOR_TMPL_LIST    (connector_tmpl_list_get_type ())
#define CONNECTOR_TMPL_LIST(obj)    (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_CONNECTOR_TMPL_LIST, ConnectorTmplList))
#define CONNECTOR_TMPL_LIST_CLASS(obj)  (G_TYPE_CHECK_CLASS_CAST ((obj), TYPE_CONNECTOR_TMPL_LIST, ConnectorTmplListClass))
#define IS_CONNECTOR_TMPL_LIST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_CONNECTOR_TMPL_LIST))
#define IS_CONNECTOR_TMPL_LIST_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE ((obj), TYPE_CONNECTOR_TMPL_LIST))
#define CONNECTOR_TMPL_LIST_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_CONNECTOR_TMPL_LIST, ConnectorTmplListClass))

typedef struct _ConnectorTmplList ConnectorTmplList;
typedef struct _ConnectorTmplListClass ConnectorTmplListClass;

struct _ConnectorTmplList
{
    GtkListStore parent;
};

struct _ConnectorTmplListClass
{
    GtkListStoreClass parent_class;
};

GType connector_tmpl_list_get_type (void);

G_END_DECLS

/**
 * Shows list of available connectors. Allows
 * adding new and modifying existing connectors.
 */
class GuiConnectors : public GuiModalDialog
{
public:
    /**
     * Create the dialog.
     * @param the parent window
     * @param ui the user interface builder
     * @param length the length of the model
     * @param width the width of the model
     */
    GuiConnectors (GtkWindow *parent, GtkBuilder *ui, const u_int16 & length, const u_int16 & width);

    /**
     * Destroy the dialog.
     */
    virtual ~GuiConnectors();

    /**
     * Update selected template.
     * @param selection the tree selection.
     */
    void setSelectedTemplate (GtkTreeSelection *selection);

    /**
     * Return the selected template.
     */
    MdlConnectorTemplate *selectedTemplate () const;

    /**
     * Add new connector template to list.
     */
    void addConnector ();

    /**
     * Remove connector template from list.
     */
    void removeConnector ();

    /**
     * Filter list to matching connectors, i.e. connectors
     * that have the same length and width as the model.
     * @param filter whether to enable or disable the filter.
     */
    void filterConnectors (const bool & filter);

private:
    /// the user interface
    GtkBuilder *Ui;
    /// the selected template
    MdlConnectorTemplate *Selected;
};

#endif /* GUI_CONNECTORS_H_ */
