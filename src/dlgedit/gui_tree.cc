/*
   $Id: gui_tree.cc,v 1.1 2004/07/25 15:52:23 ksterker Exp $

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

#define INSERT_NODE(parent,sibling,text,idx,is_leaf) \
    gtk_ctree_insert_node (GTK_CTREE (tree), parent, sibling, text, 4, \
        icon[idx], mask[idx], icon[idx], mask[idx], is_leaf, true)

/* Icon for unselected dialogue */
static const char * dlg_xpm[] = {
"16 16 5 1",
" 	c None",
".	c #646464",
"+	c #000000",
"@	c #848484",
"#	c #FFFFFF",
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
" 	c None",
".	c #446484",
"+	c #000020",
"@	c #6484A4",
"#	c #CFEFEF",
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
" 	c None",
".	c #646464",
"+	c #000000",
"@	c #848484",
"#	c #FFFFFF",
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
" 	c None",
".	c #446484",
"+	c #000020",
"@	c #6484A4",
"#	c #CFEFEF",
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
" 	c None",
".	c #004F8C",
"+	c #000000",
"@	c #EFE8EF",
"#	c #C4D9FF",
"$	c #93BCFF",
"%	c #619CCC",
"&	c #83A0D3",
"*	c #013E63",
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
void on_tree_select_row (GtkCTree *ctree, GList *node, gint column, gpointer user_data)
{
    gtk_ctree_unselect (ctree, GTK_CTREE_NODE (node));
    
    DlgModule *module = (DlgModule *) gtk_ctree_node_get_row_data (ctree,
        GTK_CTREE_NODE (node));
    
    DlgModule *current = GuiDlgedit::window->graph ()->getAttached ();
    
    if (module != current)
    {
        // unmark current selection
        GuiTree *view = (GuiTree *) user_data;
        view->select (GTK_CTREE_NODE (node));
        
        // display the newly selected module
        GuiDlgedit::window->graph ()->switchModule (module);
    }
}

// create tree widget
GuiTree::GuiTree (GtkWidget *paned)
{
    GtkWidget *scrolledwindow;

    // the scrolled window for the tree
    scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_ref (scrolledwindow);
    gtk_object_set_data_full (GTK_OBJECT (paned), "scrolledwindow",
        scrolledwindow, (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (scrolledwindow);
    gtk_paned_add1 (GTK_PANED (paned), scrolledwindow);
    gtk_widget_set_usize (scrolledwindow, 160, -2);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow),
        GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // the tree widget
    tree = gtk_ctree_new (1, 0);
    gtk_widget_ref (tree);
    gtk_object_set_data_full (GTK_OBJECT (paned), "tree", tree,
        (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show (tree);
    gtk_container_add (GTK_CONTAINER (scrolledwindow), tree);
    gtk_clist_set_column_width (GTK_CLIST (tree), 0, 80);
    gtk_clist_column_titles_hide (GTK_CLIST (tree));
    gtk_clist_set_shadow_type (GTK_CLIST (tree), GTK_SHADOW_IN);
    gtk_widget_set_can_focus(tree, FALSE);

    gtk_signal_connect (GTK_OBJECT (tree), "tree_select_row",
        GTK_SIGNAL_FUNC (on_tree_select_row), this);
    
    // create pixmaps and masks
    GdkWindow *wnd = gtk_widget_get_window(GuiDlgedit::window->getWindow ());
	icon[BUBBLE] = gdk_pixmap_create_from_xpm_d (wnd, &mask[BUBBLE], NULL, (char**) dlg_xpm);
	icon[BUBBLE_SEL] = gdk_pixmap_create_from_xpm_d (wnd, &mask[BUBBLE_SEL], NULL, (char**) sel_xpm);
	icon[BUBBLE_MOD] = gdk_pixmap_create_from_xpm_d (wnd, &mask[BUBBLE_MOD], NULL, (char**) dlg_mod_xpm);
	icon[BUBBLE_SEL_MOD] = gdk_pixmap_create_from_xpm_d (wnd, &mask[BUBBLE_SEL_MOD], NULL, (char**) sel_mod_xpm);
	icon[PROJECT] = gdk_pixmap_create_from_xpm_d (wnd, &mask[PROJECT], NULL, (char**) project_xpm);
    
    selected = NULL;
    
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
    // is module valid?
    if (module == NULL) return;
    if (locate (module) != NULL) return;
    
    // locate the project node for this module
    GtkCTreeNode* project = locateProject (module->entry ()->project ());
    
    // build the sub-tree
    build (insert (project, NULL, module));
    gtk_ctree_expand (GTK_CTREE (tree), project);
}

// find a given project
GtkCTreeNode *GuiTree::locateProject (const std::string &project)
{
    // get the root node (which must be the "none" project!)
    GtkCTreeNode *node, *root = gtk_ctree_node_nth (GTK_CTREE (tree), 0);
    GList *projects = gtk_ctree_find_all_by_row_data (GTK_CTREE (tree), root, NULL);
    gchar *text;
    
    for (; projects != NULL; projects = projects->next)
    {
        node = (GtkCTreeNode *) projects->data;
        gtk_ctree_get_node_info (GTK_CTREE (tree), node, &text, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
        if (project == text) return node;
    }
    
    // project not in tree yet -> add it
    text = (char *) project.c_str ();
    node = INSERT_NODE (NULL, NULL, &text, PROJECT, false);
    gtk_ctree_node_set_row_data (GTK_CTREE (tree), node, (gpointer) NULL);
    gtk_ctree_node_set_selectable (GTK_CTREE (tree), node, false);
    
    return node;
}

// update the given module
void GuiTree::updateModule (DlgModule *module)
{
    if (module == NULL) return;

    GtkCTreeNode *node = locate (module);
    if (node == NULL) return;
    
    GtkCTreeNode *parent = GTK_CTREE_ROW (node)->parent;
    GtkCTreeNode *sibling = GTK_CTREE_ROW (node)->sibling;
    
    // update node
    gtk_clist_freeze (GTK_CLIST (tree));
    gtk_ctree_remove_node (GTK_CTREE (tree), node);
    if (node == selected) selected = NULL;
    
    gchar *project;
    gtk_ctree_get_node_info (GTK_CTREE (tree), parent, &project, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

    // module still belongs to same project -> insert at old position
    if (module->entry ()->project () == project)
        build (insert (parent, sibling, module));
    // otherwise just append to new project
    else 
        addModule (module);        

    gtk_clist_thaw (GTK_CLIST (tree));
}

// remove the given module
void GuiTree::removeModule (DlgModule *module)
{
    if (module == NULL) return;

    GtkCTreeNode *node = locate (module);
    if (node == NULL) return;
    
    if (node == selected) selected = NULL;
    gtk_ctree_remove_node (GTK_CTREE (tree), node);
}

// locate the node corresponding to the given module
GtkCTreeNode *GuiTree::locate (DlgModule *module)
{
    GtkCTreeNode *root = gtk_ctree_node_nth (GTK_CTREE (tree), 0);
    if (root == NULL) return NULL;

    return gtk_ctree_find_by_row_data (GTK_CTREE (tree), root, (gpointer) module);
}

// add available projects to tree
void GuiTree::addProjects ()
{
    std::vector<std::string> projects = CfgData::data->projectsFromDatadir ();
    std::vector<std::string>::iterator i;
    GtkCTreeNode *node;
    gchar *text;
    
    for (i = projects.begin (); i != projects.end (); i++)
    {
        text = ((char *) (*i).c_str ());
        node = INSERT_NODE (NULL, NULL, &text, PROJECT, false);
        gtk_ctree_node_set_row_data (GTK_CTREE (tree), node, (gpointer) NULL);
        gtk_ctree_node_set_selectable (GTK_CTREE (tree), node, false);
    }    
}

// assign module to a different project
void GuiTree::updateProject (DlgModule *module)
{
    GtkCTreeNode *project = locateProject (module->entry ()->project ());
    GtkCTreeNode *node = locate (module);
    
    gtk_ctree_move (GTK_CTREE (tree), node, project, NULL);
    gtk_ctree_expand (GTK_CTREE (tree), project);
}

// build the initial tree
void GuiTree::build (GtkCTreeNode *root)
{
    DlgModule *module = (DlgModule *) gtk_ctree_node_get_row_data (GTK_CTREE (tree), root);
    std::vector<DlgNode*> nodes = module->getNodes ();
    
    // search through all nodes of the given module
    for (std::vector<DlgNode*>::iterator i = nodes.begin (); i != nodes.end (); i++)
        // if we find a sub-dialogue
        if ((*i)->type () == MODULE)
            // insert it into the tree and continue with its sub-dialogues
            build (insert (root, NULL, (DlgModule *) *i));
}

// insert a module into the tree       
GtkCTreeNode *GuiTree::insert (GtkCTreeNode *parent, GtkCTreeNode *sibling, DlgModule *module)
{
    if (parent == NULL) return NULL;
    
    gchar *text = ((char*) module->name ().c_str ());
    GtkCTreeNode *node = INSERT_NODE (parent, sibling, &text, BUBBLE, false);
    gtk_ctree_node_set_row_data (GTK_CTREE (tree), node, (gpointer) module);
    
    // display the module that has been in the view before
    if (module->displayed ())
        on_tree_select_row (GTK_CTREE (tree), (GList*) node, 0, this); 
    
    return node;
}
 
// insert a module into the tree       
void GuiTree::insert (DlgModule *parent, DlgModule *module)
{
    insert (locate (parent), NULL, module);
}

// highlight node in view
void GuiTree::select (GtkCTreeNode *node)
{
    DlgModule *module;
    
    if (node == selected) return;
        
    // first of all, deselect old node
    if (selected != NULL)
    {
        GdkColor black = { 0, 0, 0, 0 };
        gtk_ctree_node_set_foreground (GTK_CTREE (tree), selected, &black);
        module = (DlgModule *) gtk_ctree_node_get_row_data (GTK_CTREE (tree), selected);
        setIcon (selected, false, module->changed ());
    }
    
    // then select the new node
    if (node != NULL)
    {
        GdkColor blue = { 0, 0, 0, 65535 };
        gtk_ctree_node_set_foreground (GTK_CTREE (tree), node, &blue);
        module = (DlgModule *) gtk_ctree_node_get_row_data (GTK_CTREE (tree), node);
        setIcon (node, true, module->changed ());
        
        // expand parent nodes, so that selected node becomes visible
        GtkCTreeNode *parent = GTK_CTREE_ROW (node)->parent;
        
        while (parent != NULL)
        {
            gtk_ctree_expand (GTK_CTREE (tree), parent);
            parent = GTK_CTREE_ROW (parent)->parent;
        }
    }
    
    // remember selection
    selected = node;
}

// change icon of node
void GuiTree::setIcon (GtkCTreeNode *node, bool select, bool changed)
{
    int index;
    gchar *text;
    guint8 spacing;
    gboolean is_leaf;
    gboolean expanded;

    gtk_ctree_get_node_info (GTK_CTREE (tree), node, &text, &spacing,
        NULL, NULL, NULL, NULL, &is_leaf, &expanded);

    if (select) index = changed ? BUBBLE_SEL_MOD : BUBBLE_SEL;
    else index = changed ? BUBBLE_MOD : BUBBLE;
        
    gtk_ctree_set_node_info (GTK_CTREE (tree), node, text, spacing,
        icon[index], mask[index], icon[index], mask[index], is_leaf, expanded); 
}

// select the given module
void GuiTree::select (DlgModule *module)
{
    select (locate (module));        
}

// set whether a module is modified or not
void GuiTree::setChanged (DlgModule *module)
{
    // locate module
    GtkCTreeNode *node = locate (module);
    if (node == NULL) return;

    // set icon
    setIcon (node, node == selected, module->changed ());
}

// update the name of the given module
void GuiTree::setName (DlgModule *module)
{
    // locate module
    GtkCTreeNode *node = locate (module);
    if (node == NULL) return;
    
    // set name
    gchar *text = ((char*) module->name ().c_str ());
    gtk_ctree_node_set_text (GTK_CTREE (tree), node, 0, text);
}

// remove everything from the tree
void GuiTree::clear ()
{
    GtkCTreeNode *root = gtk_ctree_node_nth (GTK_CTREE (tree), 0);
    
    while (root != NULL)
    {
        gtk_ctree_remove_node (GTK_CTREE (tree), root);
        root = gtk_ctree_node_nth (GTK_CTREE (tree), 0);
    }
    
    // no tree, so nothing selected
    selected = NULL;
}
