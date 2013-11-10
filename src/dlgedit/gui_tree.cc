/*
   Copyright (C) 2002/2003 Kai Sterker <kaisterker@linuxgames.com>
   Part of the Adonthell Project http://adonthell.linuxgames.com

   Dlgedit is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Dlgedit is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Dlgedit; if not, write to the Free Software 
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

/** 
 * @file gui_tree.cc
 *
 * @author Kai Sterker
 * @brief View for the dialogue structure
 */

#include <gtk/gtk.h>
#include "cfg_data.h"
#include "gui_tree.h"
#include "gui_dlgedit.h"

/* Icon for unselected dialogue */
static const char * dlg_xpm[] = {
"16 16 5 1",
"   c None",
".  c #646464",
"+  c #000000",
"@  c #848484",
"#  c #FFFFFF",
"                ",
"                ",
"  .+++++++++.   ",
" .+@#######@+.  ",
" +@#########@+. ",
" +###########+. ",
" +###########+. ",
" +@#########@+. ",
" .+@#######@+.  ",
"  .+@#@+++++.   ",
"    +@+.....    ",
"    ++.         ",
"    +.          ",
"    .           ",
"                ",
"                "};

/* Icon for selected dialogue */
static const char * sel_xpm[] = {
"16 16 5 1",
"   c None",
".  c #446484",
"+  c #000020",
"@  c #6484A4",
"#  c #CFEFEF",
"                ",
"                ",
"  .+++++++++.   ",
" .+@#######@+.  ",
" +@#########@+. ",
" +###########+. ",
" +###########+. ",
" +@#########@+. ",
" .+@#######@+.  ",
"  .+@#@+++++.   ",
"    +@+.....    ",
"    ++.         ",
"    +.          ",
"    .           ",
"                ",
"                "};

/* Not selected and modified */
static const char * dlg_mod_xpm[] = {
"16 16 5 1",
"   c None",
".  c #646464",
"+  c #000000",
"@  c #848484",
"#  c #FFFFFF",
"                ",
"                ",
"  .+++++++++.   ",
" .+@#######@+.  ",
" +@#.@#.##@#@+. ",
" +###.##.@.##+. ",
" +###@#@#####+. ",
" +@#@.@.@.@#@+. ",
" .+@#######@+.  ",
"  .+@#@+++++.   ",
"    +@+.....    ",
"    ++.         ",
"    +.          ",
"    .           ",
"                ",
"                "};

/* Selected and modified */
static const char * sel_mod_xpm[] = {
"16 16 5 1",
"   c None",
".  c #446484",
"+  c #000020",
"@  c #6484A4",
"#  c #CFEFEF",
"                ",
"                ",
"  .+++++++++.   ",
" .+@#######@+.  ",
" +@#.@#.##@#@+. ",
" +###.##.@.##+. ",
" +###@#@#####+. ",
" +@#@.@.@.@#@+. ",
" .+@#######@+.  ",
"  .+@#@+++++.   ",
"    +@+.....    ",
"    ++.         ",
"    +.          ",
"    .           ",
"                ",
"                "};

/* Project icon */
static const char * project_xpm[] = {
"16 16 9 1",
"   c None",
".  c #004F8C",
"+  c #000000",
"@  c #EFE8EF",
"#  c #C4D9FF",
"$  c #93BCFF",
"%  c #619CCC",
"&  c #83A0D3",
"*  c #013E63",
"                ",
"  .....+        ",
" .@##$$.+       ",
".%%%%%%%......  ",
".###########$%+ ",
".#$$.%.%.%.$&%+ ",
".#$.$@@@#@$.$%+ ",
".#$.@#@#@$@*&%+ ",
".#$.$###$#&.$%+ ",
".#$$.$#*.*.$&%+ ",
".#$$$.*&$&$&&%+ ",
".#&$&*&$&$&&&%+ ",
".%%%%%%%%%%%%%+ ",
" ++++++++++++++ ",
"                ",
"                "};

// selection changed
void on_tree_select_row (GtkTreeSelection *selection, gpointer user_data)
{
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    // anything selected at all?
    if (gtk_tree_selection_get_selected (selection, &model, &iter))
    {
        DlgModule *module;
        DlgModule *current = GuiDlgedit::window->graph ()->getAttached ();

        gtk_tree_model_get (GTK_TREE_MODEL (model), &iter, 1, &module, -1);
        if (module != NULL)
        {
            if (module != current)
            {
                // mark current selection
                GuiTree *view = (GuiTree *) user_data;
                view->unselect (current);
                view->select (&iter);

                // display the newly selected module
                GuiDlgedit::window->graph ()->switchModule (module);
            }
        }
        else
        {
            gtk_tree_selection_unselect_iter (selection, &iter);
        }
    }
}

// create tree widget
GuiTree::GuiTree (GtkWidget *paned)
{
    GtkWidget *scrolledwindow;

    // the scrolled window for the tree
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    g_object_ref (scrolledwindow);
    g_object_set_data_full (G_OBJECT (paned), "scrolledwindow",
        scrolledwindow, (GDestroyNotify)  g_object_unref);
    gtk_widget_show (scrolledwindow);
    gtk_paned_add1 (GTK_PANED (paned), scrolledwindow);
    gtk_widget_set_size_request (scrolledwindow, 160, -1);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // the model
    GtkTreeStore *model = gtk_tree_store_new(3, G_TYPE_STRING, G_TYPE_POINTER, GDK_TYPE_PIXBUF);

    // the tree widget
    tree = gtk_tree_view_new_with_model (GTK_TREE_MODEL(model));
    g_object_ref (tree);
    g_object_set_data_full (G_OBJECT (paned), "tree", tree, (GDestroyNotify)  g_object_unref);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), tree);
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW(tree), FALSE);
    gtk_widget_set_can_focus(tree, FALSE);

    // create the columns
    GtkTreeViewColumn *col = gtk_tree_view_column_new();
    GtkCellRenderer *renderer = gtk_cell_renderer_pixbuf_new();
    gtk_tree_view_column_pack_start(col, renderer, FALSE);
    gtk_tree_view_column_set_attributes(col, renderer, "pixbuf", 2, NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_column_pack_start(col, renderer, TRUE);
    gtk_tree_view_column_set_attributes(col, renderer, "text", 0, NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), col);
    gtk_widget_show_all (tree);

    // connect callbacks
    GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW(tree));
    g_signal_connect (G_OBJECT (selection), "changed", G_CALLBACK(on_tree_select_row), this);
    
    // create pixmaps and masks
	icon[BUBBLE] = gdk_pixbuf_new_from_xpm_data (dlg_xpm);
	icon[BUBBLE_SEL] = gdk_pixbuf_new_from_xpm_data (sel_xpm);
	icon[BUBBLE_MOD] = gdk_pixbuf_new_from_xpm_data (dlg_mod_xpm);
	icon[BUBBLE_SEL_MOD] = gdk_pixbuf_new_from_xpm_data (sel_mod_xpm);
	icon[PROJECT] = gdk_pixbuf_new_from_xpm_data (project_xpm);

    // add available projects to tree
    addProjects ();
}

// dtor
GuiTree::~GuiTree ()
{
    clear ();
}

// add the given module
void GuiTree::addModule (DlgModule *module)
{
    GtkTreeIter node;

    // is module valid?
    if (module == NULL) return;
    if (locate (module, &node)) return;
    
    // locate the project node for this module
    GtkTreeIter project = locateProject (module->entry ()->project ());
    
    // build the sub-tree
    build (insert (project, module));

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), &project);
    gtk_tree_view_expand_to_path(GTK_TREE_VIEW(tree), path);

    // cleanup
    gtk_tree_path_free (path);
}

// find a given project
GtkTreeIter GuiTree::locateProject (const std::string &project)
{
    GtkTreeIter node;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

    if (gtk_tree_model_get_iter_first (model, &node))
    {
        gchar *text;
        do
        {
            gtk_tree_model_get (model, &node, 0, &text, -1);
            if (project == text) return node;
        }
        while (gtk_tree_model_iter_next(model, &node));
    }

    // project not in tree yet -> add it
    gtk_tree_store_append(GTK_TREE_STORE(model), &node, NULL);
    gtk_tree_store_set(GTK_TREE_STORE(model), &node, 0, project.c_str (), 1, NULL, 2, icon[PROJECT], -1);
    
    return node;
}

// update the given module
void GuiTree::updateModule (DlgModule *module)
{
    if (module == NULL) return;

    GtkTreeIter node;
    if (!locate (module, &node)) return;
    
    gchar *project;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    gtk_tree_model_get (model, &node, 0, &project, -1);

    // module still belongs to same project -> nothing to do
    if (module->entry ()->project () == project)
    {
        return;
    }
    else 
    {
        // otherwise just append to new project
        gtk_tree_store_remove(GTK_TREE_STORE(model), &node);

        addModule (module);
    }
}

// remove the given module
void GuiTree::removeModule (DlgModule *module)
{
    if (module == NULL) return;

    GtkTreeIter node;
    if (!locate (module, &node)) return;
    
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    gtk_tree_store_remove(GTK_TREE_STORE (model), &node);
}

// locate the node corresponding to the given module
bool GuiTree::locate (DlgModule *module, GtkTreeIter *node)
{
    GtkTreeIter root;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

    if (gtk_tree_model_get_iter_first (model, &root))
    {
        DlgModule *data;
        do
        {
            if (gtk_tree_model_iter_children(model, node, &root))
            {
                do
                {
                    gtk_tree_model_get (model, node, 1, &data, -1);
                    if (module == data) return true;
                }
                while (gtk_tree_model_iter_next(model, node));
            }
        }
        while (gtk_tree_model_iter_next(model, &root));
    }

    return false;
}

// add available projects to tree
void GuiTree::addProjects ()
{
    std::vector<std::string> projects = CfgData::data->projectsFromDatadir ();
    std::vector<std::string>::iterator i;

    GtkTreeIter node;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    
    for (i = projects.begin (); i != projects.end (); i++)
    {
        gtk_tree_store_append(GTK_TREE_STORE(model), &node, NULL);
        gtk_tree_store_set(GTK_TREE_STORE(model), &node, 0, i->c_str (), 1, NULL, 2, icon[PROJECT], -1);
    }
}

// assign module to a different project
void GuiTree::updateProject (DlgModule *module)
{
    GtkTreeIter node;
    GtkTreeIter project = locateProject (module->entry ()->project ());

    if (!locate (module, &node)) return;

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    gtk_tree_store_remove(GTK_TREE_STORE(model), &node);

    insert (project, module);
}

// build the initial tree
void GuiTree::build (GtkTreeIter root)
{
    DlgModule *module;

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    gtk_tree_model_get (model, &root, 1, &module, -1);
    std::vector<DlgNode*> nodes = module->getNodes ();
    
    // search through all nodes of the given module
    for (std::vector<DlgNode*>::iterator i = nodes.begin (); i != nodes.end (); i++)
        // if we find a sub-dialogue
        if ((*i)->type () == MODULE)
            // insert it into the tree and continue with its sub-dialogues
            build (insert (root, (DlgModule *) *i));
}

// insert a module into the tree       
GtkTreeIter GuiTree::insert (GtkTreeIter parent, DlgModule *module)
{
    GtkTreeIter node;
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

    gtk_tree_store_append(GTK_TREE_STORE(model), &node, &parent);
    gtk_tree_store_set(GTK_TREE_STORE(model), &node, 0, module->name ().c_str (), 1, module, 2, icon[BUBBLE], -1);

    // display the module that has been in the view before
    if (module->displayed ())
    {
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
        gtk_tree_selection_select_iter(selection, &node);
    }
    
    return node;
}
 
// insert a module into the tree       
void GuiTree::insert (DlgModule *parent, DlgModule *module)
{
    GtkTreeIter node;
    if (!locate (parent, &node)) return;

    insert (node, module);
}

// highlight node in view
void GuiTree::select (GtkTreeIter *node)
{
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));
        
    // first of all, deselect old node
    if (!gtk_tree_selection_iter_is_selected (selection, node) &&
         gtk_tree_selection_count_selected_rows(selection) == 1)
    {
        DlgModule *module;
        GtkTreeModel *model;
        GtkTreeIter selected;

        gtk_tree_selection_get_selected (selection, &model, &selected);
        gtk_tree_model_get (GTK_TREE_MODEL (model), &selected, 1, &module, -1);

        setIcon (selected, false, module->changed ());
    }
    
    // then select the new node
    if (node != NULL)
    {
        DlgModule *module;
        GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));

        gtk_tree_model_get (GTK_TREE_MODEL (model), node, 1, &module, -1);
        setIcon (*node, true, module->changed ());
        
        if (!gtk_tree_selection_iter_is_selected (selection, node))
        {
            gtk_tree_selection_select_iter(selection, node);

            // expand parent nodes, so that selected node becomes visible
            GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(model), node);
            gtk_tree_view_expand_to_path(GTK_TREE_VIEW(tree), path);
            gtk_tree_path_free (path);
        }
    }
}

// change icon of node
void GuiTree::setIcon (GtkTreeIter node, bool select, bool changed)
{
    int index;

    if (select) index = changed ? BUBBLE_SEL_MOD : BUBBLE_SEL;
    else index = changed ? BUBBLE_MOD : BUBBLE;

    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    gtk_tree_store_set (GTK_TREE_STORE (model), &node, 2, icon[index], -1);
}

// select the given module
void GuiTree::select (DlgModule *module)
{
    GtkTreeIter node;
    if (!locate (module, &node)) return;
    select (&node);
}

// deselect the given module
void GuiTree::unselect (DlgModule *module)
{
    GtkTreeIter node;
    if (!locate (module, &node)) return;

    setIcon (node, false, module->changed ());
}

// set whether a module is modified or not
void GuiTree::setChanged (DlgModule *module)
{
    // locate module
    GtkTreeIter node;
    if (!locate (module, &node)) return;

    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tree));

    // set icon
    setIcon (node, gtk_tree_selection_iter_is_selected(selection, &node), module->changed ());
}

// update the name of the given module
void GuiTree::setName (DlgModule *module)
{
    // locate module
    GtkTreeIter node;
    if (!locate (module, &node)) return;
    
    // set name
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    gtk_tree_store_set (GTK_TREE_STORE (model), &node, 0, module->name ().c_str (), -1);
}

// remove everything from the tree
void GuiTree::clear ()
{
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree));
    gtk_tree_store_clear(GTK_TREE_STORE(model));
}
