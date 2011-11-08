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
 * @file modeller/gui_connectors.cc
 *
 * @author Kai Sterker
 * @brief Dialog for editing connector templates.
 */

#include <glib.h>

#include <base/base.h>

#include "gui_connectors.h"

enum
{
    NAME_COLUMN,
    LENGTH_COLUMN,
    WIDTH_COLUMN,
    NUM_COLUMNS
};

static void connector_tmpl_list_tree_model_iface_init (GtkTreeModelIface *iface);
static int connector_tmpl_list_get_n_columns (GtkTreeModel *self);
static GType connector_tmpl_list_get_column_type (GtkTreeModel *self, int column);
static void connector_tmpl_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value);

// ConnectorTmplList inherits from GtkListStore, and implements the GtkTreeStore interface
G_DEFINE_TYPE_EXTENDED (ConnectorTmplList, connector_tmpl_list, GTK_TYPE_LIST_STORE, 0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, connector_tmpl_list_tree_model_iface_init));

// our parent's model iface
static GtkTreeModelIface parent_tmpl_iface = { { 0 }, };

// this method is called once to set up the class
static void connector_tmpl_list_class_init (ConnectorTmplListClass *klass)
{
}

// this method is called once to set up the interface
static void connector_tmpl_list_tree_model_iface_init (GtkTreeModelIface *iface)
{
    // this is where we override the interface methods
    // first make a copy of our parent's interface to call later
    parent_tmpl_iface = *iface;

    // now put in our own overriding methods
    iface->get_n_columns = connector_tmpl_list_get_n_columns;
    iface->get_column_type = connector_tmpl_list_get_column_type;
    iface->get_value = connector_tmpl_list_get_value;
}

// this method is called every time an instance of the class is created
static void connector_tmpl_list_init (ConnectorTmplList *self)
{
    // initialise the underlying storage for the GtkListStore
    GType types[] = { G_TYPE_POINTER };

    gtk_list_store_set_column_types (GTK_LIST_STORE (self), 1, types);
}

// retrieve an object from our parent's data storage
static MdlConnectorTemplate *connector_tmpl_list_get_object (ConnectorTmplList *self, GtkTreeIter *iter)
{
    GValue value = { 0, };
    MdlConnectorTemplate *obj = NULL;

    // validate our parameters
    g_return_val_if_fail (IS_CONNECTOR_TMPL_LIST (self), NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    // retrieve the object using our parent's interface, take our own reference to it
    parent_tmpl_iface.get_value (GTK_TREE_MODEL (self), iter, 0, &value);
    obj = (MdlConnectorTemplate*) g_value_get_pointer (&value);

    // cleanup
    g_value_unset (&value);

    return obj;
}

// this method returns the number of columns in our tree model
static int connector_tmpl_list_get_n_columns (GtkTreeModel *self)
{
    return NUM_COLUMNS;
}

// this method returns the type of each column in our tree model
static GType connector_tmpl_list_get_column_type (GtkTreeModel *self, int column)
{
    static GType types[] = {
        G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT
    };

    // validate our parameters
    g_return_val_if_fail (IS_CONNECTOR_TMPL_LIST (self), G_TYPE_INVALID);
    g_return_val_if_fail (column >= 0 && column < NUM_COLUMNS, G_TYPE_INVALID);

    return types[column];
}

// this method retrieves the value for a particular column
static void connector_tmpl_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value)
{
    // validate our parameters
    g_return_if_fail (IS_CONNECTOR_TMPL_LIST (self));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column >= 0 && column < NUM_COLUMNS);
    g_return_if_fail (value != NULL);

    // get the object from our parent's storage
    MdlConnectorTemplate *obj = (MdlConnectorTemplate*) connector_tmpl_list_get_object (CONNECTOR_TMPL_LIST (self), iter);

    // initialise our GValue to the required type
    g_value_init (value, connector_tmpl_list_get_column_type (GTK_TREE_MODEL (self), column));

    switch (column)
    {
        case NAME_COLUMN:
        {
            const gchar *name = obj->name().c_str();
            g_value_set_string (value, name);
            break;
        }
        case LENGTH_COLUMN:
        {
            g_value_set_int (value, obj->length());
            break;
        }
        case WIDTH_COLUMN:
        {
            g_value_set_int (value, obj->width());
            break;
        }
        default:
        {
            g_assert_not_reached ();
        }
    }
}


static gboolean on_window_closed (GtkWidget *widget, GdkEvent *event, gpointer user_data)
{
    gtk_main_quit ();

    return TRUE;
}

// close the dialog and keep all changes
static void on_ok_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiConnectors *dialog = (GuiConnectors *) user_data;
    dialog->okButtonPressed (true);

    // save
    MdlConnectorManager::save (base::Paths().user_data_dir());

    // clean up
    gtk_main_quit ();
}

// close dialog and dismiss all changes
static void on_cancel_button_pressed (GtkButton * button, gpointer user_data)
{
    // clean up
    gtk_main_quit ();
}

// add or create a new template
static void on_add_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiConnectors *dialog = (GuiConnectors *) user_data;
    dialog->addConnector ();
}

static void on_remove_button_pressed (GtkEditable *editable, gpointer user_data)
{
    GuiConnectors *dialog = (GuiConnectors *) user_data;
    dialog->removeConnector ();
}

static void on_filter_button_toggled (GtkToggleButton * button, gpointer user_data)
{
    GuiConnectors *dialog = (GuiConnectors *) user_data;
    dialog->filterConnectors (gtk_toggle_button_get_active (button));
}

// callback for selection changes
static void selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GuiConnectors *dialog = (GuiConnectors *) user_data;
    dialog->setSelectedTemplate(selection);
}

// connector template edited
static void template_edited_event (GtkCellRendererText *cell, gchar *path, gchar *new_val, gpointer user_data)
{
    GtkTreeIter iter;
    GtkTreeModel *model = GTK_TREE_MODEL(user_data);

    if (gtk_tree_model_get_iter_from_string (model, &iter, path))
    {
        MdlConnectorTemplate *tmpl = connector_tmpl_list_get_object(CONNECTOR_TMPL_LIST(model), &iter);

        guint col = GPOINTER_TO_UINT(g_object_get_data(G_OBJECT(cell), "column_index"));
        switch (col)
        {
            case NAME_COLUMN:
            {
                tmpl->set_name(new_val);
                break;
            }
            case LENGTH_COLUMN:
            {
                s_int16 length = (s_int16) atoi(new_val);
                tmpl->set_length(length);
                break;
            }
            case WIDTH_COLUMN:
            {
                s_int16 width = (s_int16) atoi(new_val);
                tmpl->set_width(width);
                break;
            }
        }
    }
}

bool GuiConnectors::First = true;

// ctor
GuiConnectors::GuiConnectors(GtkWindow *parent, GtkBuilder *ui, const u_int16 & length, const u_int16 & width)
: GuiModalDialog (parent)
{
    GtkTreeIter iter;
    GObject *widget;
    GtkListStore *model = NULL;

    Ui = ui;

    // get reference to dialog window
    window = GTK_WIDGET (gtk_builder_get_object (Ui, "connector_dialog"));
    Selected = NULL;

    // current model footprint
    char tmp[32];
    widget = gtk_builder_get_object (Ui, "lbl_length");
    g_object_set_data (widget, "model_length", GUINT_TO_POINTER(length));
    sprintf (tmp, "%i", length);
    gtk_label_set_text(GTK_LABEL(widget), tmp);
    widget = gtk_builder_get_object (Ui, "lbl_width");
    g_object_set_data (widget, "model_width", GUINT_TO_POINTER(width));
    sprintf (tmp, "%i", width);
    gtk_label_set_text(GTK_LABEL(widget), tmp);

    if (First)
    {
        // make sure the window is closed, but not deleted when pressing the close button
        // of the window border
        g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(on_window_closed), NULL);

        // setup button callbacks
        widget = gtk_builder_get_object (Ui, "btn_okay");
        g_signal_connect (widget, "clicked", G_CALLBACK (on_ok_button_pressed), this);
        widget = gtk_builder_get_object (Ui, "btn_cancel");
        g_signal_connect (widget, "clicked", G_CALLBACK (on_cancel_button_pressed), this);
        widget = gtk_builder_get_object (Ui, "btn_tmpl_add");
        g_signal_connect (widget, "clicked", G_CALLBACK (on_add_button_pressed), this);
        widget = gtk_builder_get_object (Ui, "btn_tmpl_remove");
        g_signal_connect (widget, "clicked", G_CALLBACK (on_remove_button_pressed), this);
        widget = gtk_builder_get_object (Ui, "btn_filter");
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (widget), false);
        g_signal_connect (widget, "toggled", G_CALLBACK (on_filter_button_toggled), this);

        // create the connector template model
        widget = gtk_builder_get_object (Ui, "view_available_connectors");
        model = (GtkListStore *) g_object_new (TYPE_CONNECTOR_TMPL_LIST, NULL);
        gtk_tree_view_set_model (GTK_TREE_VIEW(widget), (GtkTreeModel*) model);

        // add selection listener
        GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
        g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(selected_event), this);

        // add cell edit listeners
        widget = gtk_builder_get_object (Ui, "rnd_name");
        g_object_set_data (widget, "column_index", GUINT_TO_POINTER(NAME_COLUMN));
        g_signal_connect (G_OBJECT(widget), "edited", G_CALLBACK(template_edited_event), model);
        widget = gtk_builder_get_object (Ui, "rnd_length");
        g_object_set_data (widget, "column_index", GUINT_TO_POINTER(LENGTH_COLUMN));
        g_signal_connect (G_OBJECT(widget), "edited", G_CALLBACK(template_edited_event), model);
        widget = gtk_builder_get_object (Ui, "rnd_width");
        g_object_set_data (widget, "column_index", GUINT_TO_POINTER(WIDTH_COLUMN));
        g_signal_connect (G_OBJECT(widget), "edited", G_CALLBACK(template_edited_event), model);

        First = false;
    }

    // load and populate model
    if (model == NULL)
    {
        widget = gtk_builder_get_object (Ui, "view_available_connectors");
        model = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW(widget)));
        gtk_list_store_clear(model);
    }

    MdlConnectorManager::load(base::Paths().user_data_dir());
    for (MdlConnectorManager::iterator i = MdlConnectorManager::begin(); i != MdlConnectorManager::end(); i++)
    {
        gtk_list_store_append (model, &iter);
        gtk_list_store_set (model, &iter, 0, i->second, -1);
    }
}

// dtor
GuiConnectors::~GuiConnectors()
{
    // "close" dialog
    gtk_widget_hide (window);

    // prevent UiBuilder resources from being deleted
    window = NULL;
}

// add connector to list
void GuiConnectors::addConnector ()
{
    GObject *widget;
    GtkTreeIter iter;
    MdlConnectorTemplate *tmpl = MdlConnectorManager::create();

    // length
    widget = gtk_builder_get_object (Ui, "lbl_length");
    tmpl->set_length(GPOINTER_TO_UINT (g_object_get_data (widget, "model_length")));

    // width
    widget = gtk_builder_get_object (Ui, "lbl_width");
    tmpl->set_width(GPOINTER_TO_UINT (g_object_get_data (widget, "model_width")));

    // add to connector list
    widget = gtk_builder_get_object (Ui, "view_available_connectors");

    GtkListStore *model = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW(widget)));
    gtk_list_store_append (model, &iter);
    gtk_list_store_set (model, &iter, 0, tmpl, -1);

    // select it
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
    gtk_tree_selection_select_iter (selection, &iter);

    // and scroll into view
    GtkTreePath *path = gtk_tree_model_get_path (GTK_TREE_MODEL(model), &iter);
    gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW(widget), path, NULL, TRUE, 0.5f, 0.0f);
    gtk_tree_path_free (path);
}

// delete a connector template
void GuiConnectors::removeConnector ()
{
    if (Selected != NULL)
    {
        MdlConnectorManager::remove (Selected->uid());

        GtkTreeIter iter;
        GtkTreeModel *model;
        GtkTreeView *view = GTK_TREE_VIEW (gtk_builder_get_object (Ui, "view_available_connectors"));
        GtkTreeSelection *selection = gtk_tree_view_get_selection (view);

        gtk_tree_selection_get_selected (selection, &model, &iter);
        gtk_tree_selection_unselect_iter (selection, &iter);

        gtk_list_store_remove (GTK_LIST_STORE(model), &iter);
    }
}

// filter matching connectors
void GuiConnectors::filterConnectors (const bool & filter)
{
    GtkTreeIter iter;
    GtkTreeView *view = GTK_TREE_VIEW (gtk_builder_get_object (Ui, "view_available_connectors"));
    GtkListStore *model = GTK_LIST_STORE (gtk_tree_view_get_model (view));

    // prevent UI updates while applying filter change
    gtk_tree_view_set_model (view, NULL);

    // remove all elements from list
    gtk_list_store_clear (model);

    if (filter)
    {
        GObject *widget;

        // get length
        widget = gtk_builder_get_object (Ui, "lbl_length");
        u_int16 length = GPOINTER_TO_UINT (g_object_get_data (widget, "model_length"));

        // get width
        widget = gtk_builder_get_object (Ui, "lbl_width");
        u_int16 width = GPOINTER_TO_UINT (g_object_get_data (widget, "model_width"));

        // filter to matching templates
        for (MdlConnectorManager::iterator i = MdlConnectorManager::begin(); i != MdlConnectorManager::end(); i++)
        {
            if (length == i->second->length() && width == i->second->width())
            {
                gtk_list_store_append (model, &iter);
                gtk_list_store_set (model, &iter, 0, i->second, -1);
            }
        }
    }
    else
    {
        // show all templates
        for (MdlConnectorManager::iterator i = MdlConnectorManager::begin(); i != MdlConnectorManager::end(); i++)
        {
            gtk_list_store_append (model, &iter);
            gtk_list_store_set (model, &iter, 0, i->second, -1);
        }
    }

    // resume UI updates
    gtk_tree_view_set_model (view, GTK_TREE_MODEL (model));
}

// get the selected template
MdlConnectorTemplate *GuiConnectors::selectedTemplate () const
{
    return Selected;
}

// set the selected template
void GuiConnectors::setSelectedTemplate (GtkTreeSelection *selection)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    GObject *widget;

    // anything selected at all?
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        // get object at selected row
        Selected = (MdlConnectorTemplate*) connector_tmpl_list_get_object(CONNECTOR_TMPL_LIST(model), &iter);
    }
    else
    {
        Selected = NULL;
    }

    widget = gtk_builder_get_object (Ui, "btn_okay");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), Selected != NULL);
    widget = gtk_builder_get_object (Ui, "btn_tmpl_remove");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), Selected != NULL);
}
