## Process this file with automake to produce Makefile.in

noinst_PROGRAMS = dlgedit

# SUBDIRS = examples

EXTRA_DIST = loadlg.l loadcfg.l

dlgedit_SOURCES = \
    cfg_data.cc \
    cfg_io.cc \
    cfg_project.cc \
    dlg_arrow.cc \
    dlg_circle.cc \
    dlg_circle_entry.cc \
    dlg_cmdline.cc \
    dlg_compiler.cc \
    dlg_module.cc \
    dlg_module_entry.cc \
    dlg_mover.cc \
    dlg_node.cc \
    dlg_node_gfx.cc \
    dlg_point.cc \
    dlg_rect.cc \
    gui_circle.cc \
    gui_circle_events.cc \
    gui_code.cc \
    gui_dlgedit.cc \
    gui_dlgedit_events.cc \
    gui_edit.cc \
    gui_error.cc \
    gui_file.cc \
    gui_graph.cc \
    gui_graph_events.cc \
    gui_list.cc \
    gui_list_events.cc \
    gui_messages.cc \
    gui_modal_dialog.cc \
    gui_resources.cc \
    gui_settings.cc \
    gui_tooltip.cc \
    gui_tree.cc \
    kb_traverse.cc \
    lex.loadcfg.cc \
    lex.loadlg.cc \
    main.cc \
    cfg_data.h \
    cfg_io.h \
    cfg_project.h \
    dlg_arrow.h \
    dlg_circle.h \
    dlg_circle_entry.h \
    dlg_cmdline.h \
    dlg_compiler.h \
    dlg_module.h \
    dlg_module_entry.h \
    dlg_mover.h \
    dlg_node.h \
    dlg_node_gfx.h \
    dlg_point.h \
    dlg_rect.h \
    dlg_types.h \
    gui_circle.h \
    gui_circle_events.h \
    gui_code.h \
    gui_dlgedit.h \
    gui_dlgedit_events.h \
    gui_edit.h \
    gui_error.h \
    gui_file.h \
    gui_graph.h \
    gui_graph_events.h \
    gui_list.h \
    gui_list_events.h \
    gui_messages.h \
    gui_modal_dialog.h \
    gui_resources.h \
    gui_settings.h \
    gui_tooltip.h \
    gui_tree.h \
    kb_traverse.h

# Evil, but it works...
AM_CPPFLAGS = -D_VERSION_=\"0.9pre\" $(GTK_CFLAGS) $(ADONTHELL_CFLAGS)

dlgedit_LDADD = $(GTK_LIBS) $(ADONTHELL_LIBS)

lex.loadlg.cc: $(top_srcdir)/src/dlgedit/loadlg.l
	flex -olex.loadlg.cc $<

lex.loadcfg.cc: $(top_srcdir)/src/dlgedit/loadcfg.l
	flex -olex.loadcfg.cc $<
