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
 * @file mapedit/gui_filter_dialog.cc
 *
 * @author Kai Sterker
 * @brief Control filtering of entity list.
 */

#include <world/area_manager.h>

#include "gui_filter_dialog.h"
#include "gui_entity_list.h"
#include "gui_mapedit.h"
#include "gui_mapview.h"
#include "map_data.h"
#include "map_entity.h"

// static entity filter model instance used throughout mapedit
GtkListStore *GuiFilterDialog::FilterModel = NULL;

// active filter function
base::functor_1ret<const MapEntity*, bool> *GuiFilterDialog::FilterFunc = base::make_functor_ret (&GuiFilterDialog::filterByConnector);

// active filter type
GuiFilterDialog::filter_type GuiFilterDialog::ActiveFilter = GuiFilterDialog::BY_CONNECTOR;

// whether filter is active or not
bool GuiFilterDialog::Paused = false;

// Ui definition
static char filter_dialog_ui[] =
#include "entity-filter.glade.h"
;

static void on_close (GtkDialog *widget, gpointer user_data)
{
    GuiFilterDialog* dlg = (GuiFilterDialog*) user_data;
    dlg->close();
}

static void on_filtered_toggled (GtkCellRendererToggle *cell, gchar *path_str, gpointer data)
{
    GtkListStore* model = (GtkListStore *) data;

    GtkTreePath *path = gtk_tree_path_new_from_string (path_str);
    GtkTreeIter iter;
    bool enabled;

    gtk_tree_model_get_iter (GTK_TREE_MODEL(model), &iter, path);
    gtk_tree_model_get (GTK_TREE_MODEL(model), &iter, 0, &enabled, -1);
    gtk_list_store_set (model, &iter, 0, !enabled, -1);

    gtk_tree_path_free (path);

    // update entity list
    GuiMapedit::window->entityList()->filterChanged();
}

static void on_filter_changed (GtkToggleButton *btn, gpointer data)
{
    if (gtk_toggle_button_get_active(btn))
    {
        GuiFilterDialog* filterDlg = (GuiFilterDialog *) data;
        filterDlg->setFilter (gtk_buildable_get_name(GTK_BUILDABLE (btn)));
    }
}

// ctor
GuiFilterDialog::GuiFilterDialog (GtkListStore *filter_model)
: GuiModalDialog (GTK_WINDOW(GuiMapedit::window->getWindow()), false)
{
    GError *err = NULL;
    GObject *widget;
    
    Ui = gtk_builder_new();
    
	if (!gtk_builder_add_from_string(Ui, filter_dialog_ui, -1, &err))
    {
        g_message ("building filter dialog failed: %s", err->message);
        g_error_free (err);
        return;
    }
    
    // get reference to dialog window
    widget = gtk_builder_get_object (Ui, "filter-dialog");
    window = GTK_WIDGET (widget);
    g_signal_connect (widget, "close", G_CALLBACK (on_close), this);
    
    // set model and initial sorting
    widget = gtk_builder_get_object (Ui, "filter_view");
    gtk_tree_view_set_model (GTK_TREE_VIEW(widget), GTK_TREE_MODEL(filter_model));
    gtk_tree_sortable_set_sort_column_id (GTK_TREE_SORTABLE(filter_model), 2, GTK_SORT_DESCENDING);
    
    // hook callbacks
    widget = gtk_builder_get_object (Ui, "filter_renderer");
    g_signal_connect (G_OBJECT(widget), "toggled", G_CALLBACK(on_filtered_toggled), filter_model);

    widget = gtk_builder_get_object (Ui, "rb_filter_off");
    if (ActiveFilter == NONE) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), true);
    g_signal_connect (G_OBJECT(widget), "toggled", G_CALLBACK(on_filter_changed), this);
    widget = gtk_builder_get_object (Ui, "rb_filter_tag");
    if (ActiveFilter == BY_TAG) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), true);
    g_signal_connect (G_OBJECT(widget), "toggled", G_CALLBACK(on_filter_changed), this);
    widget = gtk_builder_get_object (Ui, "rb_filter_matching");
    if (ActiveFilter == BY_CONNECTOR) gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(widget), true);
    g_signal_connect (G_OBJECT(widget), "toggled", G_CALLBACK(on_filter_changed), this);

    // only activate tag list if filtering by tag
    widget = gtk_builder_get_object (Ui, "scrolledwindow1");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), ActiveFilter == BY_TAG);

    // move dialog out of the way
    int x, y;
    gtk_widget_show_all (window);
    gtk_window_get_position (GTK_WINDOW(window), &x, &y);
    gtk_window_move (GTK_WINDOW(window), x, y);
    gtk_window_resize (GTK_WINDOW(window), 320, 600);
}

// dtor
GuiFilterDialog::~GuiFilterDialog ()
{
    g_object_unref (Ui);
}

// when the dialog is closed
void GuiFilterDialog::close ()
{
    gtk_window_present (parent);
}

// set a different filter
void GuiFilterDialog::setFilter (const std::string & name)
{
    // cleanup first
    delete FilterFunc;
    FilterFunc = NULL;

    // only activate tag list if filtering by tag
    GObject *widget = gtk_builder_get_object (Ui, "scrolledwindow1");
    gtk_widget_set_sensitive (GTK_WIDGET (widget), name == "rb_filter_tag");

    // set filter function, if any
    if (name == "rb_filter_tag")
    {
        ActiveFilter = BY_TAG;
        FilterFunc = base::make_functor_ret (&GuiFilterDialog::filterByTag);
    }
    else if (name == "rb_filter_matching")
    {
        ActiveFilter = BY_CONNECTOR;
        FilterFunc = base::make_functor_ret (&GuiFilterDialog::filterByConnector);
    }
    else
    {
        ActiveFilter = NONE;
    }

    // update entity list
    GuiMapedit::window->entityList()->filterChanged();
}

// create a singleton filter model
GtkListStore *GuiFilterDialog::getFilterModel()
{
    if (FilterModel == NULL)
    {
        GError *err = NULL;
        gchar* model[] = { (gchar*)"filter_model", NULL };

        GtkBuilder *builder = gtk_builder_new();

        if (gtk_builder_add_objects_from_string(builder, filter_dialog_ui, -1, model, &err))
        {
            FilterModel = GTK_LIST_STORE (gtk_builder_get_object (builder, model[0]));
            gtk_list_store_clear (FilterModel);
            g_object_ref (FilterModel);
        }
        else
        {
            g_message ("creating filter model failed: %s", err->message);
            g_error_free (err);
        }

        g_object_unref (builder);
    }

    return FilterModel;
}

// check whether entity should be filtered from entity list
bool GuiFilterDialog::isEntityFiltered (const MapEntity *entity)
{
    // filter active at all?
    if (Paused) return false;

    // some common filters
    if (entity == NULL) return true;
    if (entity->hasTag ("template")) return true;
    if (entity == GuiMapedit::window->view()->getSelectedObject()) return false;

    if (FilterFunc != NULL) return (*FilterFunc)(entity);

    // if we end up here, entity will be shown
    return false;
}

// suspend filtering
void GuiFilterDialog::pauseFiltering()
{
    if (!Paused)
    {
        Paused = true;
        GuiMapedit::window->entityList()->filterChanged();
    }
}

// continue filtering
void GuiFilterDialog::resumeFiltering()
{
    if (Paused)
    {
        Paused = false;
        GuiMapedit::window->entityList()->filterChanged();
    }
}

// check for a tag in the filter's list
bool GuiFilterDialog::findTagInFilter (const gchar *tag, GtkTreeIter *result)
{
    GtkTreeModel *model = GTK_TREE_MODEL (getFilterModel());
    GtkTreeIter iter;

    // iterate over all entities in model
    if (gtk_tree_model_get_iter_first (model, &iter))
    {
        do
        {
            gchar *value = NULL;
            gtk_tree_model_get (model, &iter, 1, &value, -1);

            // found tag in model?
            if (strcmp(tag, value) == 0)
            {
                *result = iter;
                return true;
            }
        }
        while (gtk_tree_model_iter_next (model, &iter));
    }

    return false;
}

// show all entities containing one of the selected tags
bool GuiFilterDialog::filterByTag (const MapEntity *entity)
{
    GtkTreeIter filterIter;
    GtkTreeModel *filter = GTK_TREE_MODEL (GuiFilterDialog::getFilterModel());

    if (gtk_tree_model_get_iter_first (filter, &filterIter))
    {
        do
        {
            bool enabled = false;
            gchar *tag = NULL;
            gtk_tree_model_get (filter, &filterIter, 0, &enabled, 1, &tag, -1);

            // found tag in model?
            if (enabled && entity->hasTag(tag))
            {
                return false;
            }
        }
        while (gtk_tree_model_iter_next (filter, &filterIter));
    }

    return true;
}

// show all entities with matching connector
bool GuiFilterDialog::filterByConnector (const MapEntity *entity)
{
    // the object currently used for drawing
    MapEntity *curObj = GuiMapedit::window->view()->getSelectedObject();
    if (curObj == NULL) return false;

    // now check if there is at least one matching connector
    return !curObj->canConnectWith (entity, true);
}
