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

#include <base/base.h>

#include "gui_connectors.h"

enum
{
    NAME_COLUMN,
    FACE_COLUMN,
    SIZE_COLUMN,
    NUM_COLUMNS
};

static void connector_list_tree_model_iface_init (GtkTreeModelIface *iface);
static int connector_list_get_n_columns (GtkTreeModel *self);
static GType connector_list_get_column_type (GtkTreeModel *self, int column);
static void connector_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value);

// ConnectorList inherits from GtkListStore, and implements the GtkTreeStore interface
G_DEFINE_TYPE_EXTENDED (ConnectorList, connector_list, GTK_TYPE_LIST_STORE, 0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, connector_list_tree_model_iface_init));

// our parent's model iface
static GtkTreeModelIface parent_iface = { 0, };

// this method is called once to set up the class
static void connector_list_class_init (ConnectorListClass *klass)
{
}

// this method is called once to set up the interface
static void connector_list_tree_model_iface_init (GtkTreeModelIface *iface)
{
    // this is where we override the interface methods
    // first make a copy of our parent's interface to call later
    parent_iface = *iface;

    // now put in our own overriding methods
    iface->get_n_columns = connector_list_get_n_columns;
    iface->get_column_type = connector_list_get_column_type;
    iface->get_value = connector_list_get_value;
}

// this method is called every time an instance of the class is created
static void connector_list_init (ConnectorList *self)
{
    // initialise the underlying storage for the GtkListStore
    GType types[] = { G_TYPE_POINTER };

    gtk_list_store_set_column_types (GTK_LIST_STORE (self), 1, types);
}

// retrieve an object from our parent's data storage
static MdlConnector *connector_list_get_object (ConnectorList *self, GtkTreeIter *iter)
{
    GValue value = { 0, };
    MdlConnector *obj = NULL;

    // validate our parameters
    g_return_val_if_fail (IS_CONNECTOR_LIST (self), NULL);
    g_return_val_if_fail (iter != NULL, NULL);

    // retreive the object using our parent's interface, take our own reference to it
    parent_iface.get_value (GTK_TREE_MODEL (self), iter, 0, &value);
    obj = (MdlConnector*) g_value_get_pointer (&value);

    // cleanup
    g_value_unset (&value);

    return obj;
}

// this method returns the number of columns in our tree model
static int connector_list_get_n_columns (GtkTreeModel *self)
{
    return NUM_COLUMNS;
}

// this method returns the type of each column in our tree model
static GType connector_list_get_column_type (GtkTreeModel *self, int column)
{
    static GType types[] = {
        G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT
    };

    // validate our parameters
    g_return_val_if_fail (IS_CONNECTOR_LIST (self), G_TYPE_INVALID);
    g_return_val_if_fail (column >= 0 && column < NUM_COLUMNS, G_TYPE_INVALID);

    return types[column];
}

// this method retrieves the value for a particular column
static void connector_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value)
{
    // validate our parameters
    g_return_if_fail (IS_CONNECTOR_LIST (self));
    g_return_if_fail (iter != NULL);
    g_return_if_fail (column >= 0 && column < NUM_COLUMNS);
    g_return_if_fail (value != NULL);

    // get the object from our parent's storage
    MdlConnector *obj = (MdlConnector*) connector_list_get_object (CONNECTOR_LIST (self), iter);

    // initialise our GValue to the required type
    g_value_init (value, connector_list_get_column_type (GTK_TREE_MODEL (self), column));

    switch (column)
    {
        case NAME_COLUMN:
        {
            const gchar *name = obj->name().c_str();
            g_value_set_string (value, name);
            break;
        }
        case FACE_COLUMN:
        {
            switch (obj->side())
            {
                case MdlConnector::FRONT:
                    g_value_set_string (value, "Front");
                    break;
                case MdlConnector::BACK:
                    g_value_set_string (value, "Back");
                    break;
                case MdlConnector::LEFT:
                    g_value_set_string (value, "Left");
                    break;
                case MdlConnector::RIGHT:
                    g_value_set_string (value, "Right");
                    break;
                default:
                    g_assert_not_reached ();
            }

            break;
        }
        case SIZE_COLUMN:
        {
            g_value_set_int (value, obj->pos ());
            break;
        }
        default:
        {
            g_assert_not_reached ();
        }
    }
}


static void connector_tmpl_list_tree_model_iface_init (GtkTreeModelIface *iface);
static int connector_tmpl_list_get_n_columns (GtkTreeModel *self);
static GType connector_tmpl_list_get_column_type (GtkTreeModel *self, int column);
static void connector_tmpl_list_get_value (GtkTreeModel *self, GtkTreeIter *iter, int column, GValue *value);

// ConnectorTmplList inherits from GtkListStore, and implements the GtkTreeStore interface
G_DEFINE_TYPE_EXTENDED (ConnectorTmplList, connector_tmpl_list, GTK_TYPE_LIST_STORE, 0,
                        G_IMPLEMENT_INTERFACE (GTK_TYPE_TREE_MODEL, connector_tmpl_list_tree_model_iface_init));

// our parent's model iface
static GtkTreeModelIface parent_tmpl_iface = { 0, };

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

    // retreive the object using our parent's interface, take our own reference to it
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
        case FACE_COLUMN:
        {
            g_value_set_int (value, obj->length());
            break;
        }
        case SIZE_COLUMN:
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
static void on_update_button_pressed (GtkButton * button, gpointer user_data)
{
    GuiConnectors *dialog = (GuiConnectors *) user_data;
    dialog->updateConnector ();
}

// callback for selection changes
static void selected_event (GtkTreeSelection *selection, gpointer user_data)
{
    GuiConnectors *dialog = (GuiConnectors *) user_data;
    dialog->setSelectedTemplate(selection);
}

// update "commit" button state
static void on_value_changed (GtkEditable *editable, gpointer user_data)
{
    GObject *widget;
    bool all_valid = true;
    GtkBuilder *ui = (GtkBuilder*) user_data;

    // check all inputs
    widget = gtk_builder_get_object (ui, "entry_name");
    all_valid &= gtk_entry_get_text_length (GTK_ENTRY(widget)) > 0;
    widget = gtk_builder_get_object (ui, "entry_length");
    all_valid &= gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(widget)) > 0;
    widget = gtk_builder_get_object (ui, "entry_width");
    all_valid &= gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(widget)) > 0;

    // enable or disable "commit" button
    widget = gtk_builder_get_object (ui, "btn_update");
    gtk_widget_set_sensitive (GTK_WIDGET(widget), all_valid);
}

// ctor
GuiConnectors::GuiConnectors(GtkWindow *parent, GtkBuilder *ui)
: GuiModalDialog (parent)
{
    GtkAdjustment *adj;
    GtkTreeIter iter;
    GObject *widget;
    Ui = ui;

    // get reference to dialog window
    window = GTK_WIDGET (gtk_builder_get_object (Ui, "connector_dialog"));
    Selected = NULL;

    // make sure the window is closed, but not deleted when pressing the close button
    // of the window border
    g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(on_window_closed), NULL);

    // setup button callbacks
    widget = gtk_builder_get_object (Ui, "btn_okay");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_ok_button_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_cancel");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_cancel_button_pressed), this);
    widget = gtk_builder_get_object (Ui, "btn_update");
    g_signal_connect (widget, "clicked", G_CALLBACK (on_update_button_pressed), this);
    gtk_button_set_label (GTK_BUTTON(widget), "Create");

    // callbacks to check for valid values in edit fields
    widget = gtk_builder_get_object (Ui, "entry_name");
    g_signal_connect (widget, "changed", G_CALLBACK (on_value_changed), Ui);

    widget = gtk_builder_get_object (Ui, "entry_length");
    g_signal_connect (widget, "changed", G_CALLBACK (on_value_changed), Ui);
    adj = (GtkAdjustment *) gtk_adjustment_new (0, 0, 1024, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);

    widget = gtk_builder_get_object (Ui, "entry_width");
    g_signal_connect (widget, "changed", G_CALLBACK (on_value_changed), Ui);
    adj = (GtkAdjustment *) gtk_adjustment_new (0, 0, 1024, 1.0, 10.0, 0.0);
    gtk_spin_button_set_adjustment(GTK_SPIN_BUTTON(widget), adj);

    // create the connector template model
    widget = gtk_builder_get_object (Ui, "view_available_connectors");
    GtkListStore *model = (GtkListStore *) g_object_new (TYPE_CONNECTOR_TMPL_LIST, NULL);
    gtk_tree_view_set_model (GTK_TREE_VIEW(widget), (GtkTreeModel*) model);

    // add selection listener
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
    g_signal_connect (G_OBJECT(selection), "changed", G_CALLBACK(selected_event), this);

    // load and populate model
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

void GuiConnectors::updateConnector ()
{
    GObject *widget;
    GtkTreeIter iter;
    MdlConnectorTemplate *tmpl = Selected != NULL ? Selected : MdlConnectorManager::create();

    // name
    widget = gtk_builder_get_object (Ui, "entry_name");
    tmpl->set_name(gtk_entry_get_text (GTK_ENTRY(widget)));
    gtk_entry_set_text (GTK_ENTRY(widget), "");

    // length
    widget = gtk_builder_get_object (Ui, "entry_length");
    tmpl->set_length(gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(widget)));
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), 0.0);

    // width
    widget = gtk_builder_get_object (Ui, "entry_width");
    tmpl->set_width(gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON(widget)));
    gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), 0.0);

    widget = gtk_builder_get_object (Ui, "view_available_connectors");
    if (Selected != NULL)
    {
        // clear selection
        GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(widget));
        gtk_tree_selection_get_selected (selection, NULL, &iter);
        gtk_tree_selection_unselect_iter (selection, &iter);
    }
    else
    {
        // add to connector list
        GtkListStore *model = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW(widget)));
        gtk_list_store_append (model, &iter);
        gtk_list_store_set (model, &iter, 0, tmpl, -1);
    }
}

MdlConnectorTemplate *GuiConnectors::selectedTemplate () const
{
    return Selected;
}

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

        widget = gtk_builder_get_object (Ui, "entry_name");
        gtk_entry_set_text (GTK_ENTRY(widget), Selected->name().c_str());
        widget = gtk_builder_get_object (Ui, "entry_length");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), Selected->length());
        widget = gtk_builder_get_object (Ui, "entry_width");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), Selected->width());
        widget = gtk_builder_get_object (Ui, "btn_update");
        gtk_button_set_label (GTK_BUTTON(widget), "Update");
        widget = gtk_builder_get_object (Ui, "btn_okay");
        gtk_widget_set_sensitive (GTK_WIDGET(widget), true);
    }
    else
    {
        Selected = NULL;

        widget = gtk_builder_get_object (Ui, "entry_name");
        gtk_entry_set_text (GTK_ENTRY(widget), "");
        widget = gtk_builder_get_object (Ui, "entry_length");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), 0.0);
        widget = gtk_builder_get_object (Ui, "entry_width");
        gtk_spin_button_set_value (GTK_SPIN_BUTTON(widget), 0.0);
        widget = gtk_builder_get_object (Ui, "btn_update");
        gtk_button_set_label (GTK_BUTTON(widget), "Create");
        widget = gtk_builder_get_object (Ui, "btn_okay");
        gtk_widget_set_sensitive (GTK_WIDGET(widget), false);
    }
}
