## Process this file with automake to produce Makefile.in

EXTRA_DIST = entity-properties.glade

noinst_PROGRAMS = mapedit

noinst_HEADERS = \
    gui_entity_dialog.h \
    gui_entity_list.h \
    gui_grid.h \
    gui_mapedit.h \
    gui_mapedit_events.h \
    gui_mapview.h \
    gui_mapview_events.h \
    gui_renderheight.h \
    map_cmdline.h \
    map_data.h \
    map_entity.h \
    map_mgr.h \
    map_renderer.h
    
mapedit_SOURCES = \
    gui_entity_dialog.cc \
    gui_entity_list.cc \
    gui_grid.cc \
    gui_mapedit.cc \
    gui_mapedit_events.cc \
    gui_mapview.cc \
    gui_mapview_events.cc \
    gui_renderheight.cc \
    main.cc \
    map_cmdline.cc \
    map_data.cc \
    map_entity.cc \
    map_renderer.cc

# just for the dependency
gui_entity_dialog.cc : entity-properties.glade.h

# generate a string out of the glade XML
$(srcdir)/entity-properties.glade.h : entity-properties.glade
	$(SED) -e 's/"/\\"/g' -e 's/>$$/>"/' -e 's/^\( *\)</\1"</' $< > $@

INCLUDES = -I@top_srcdir@/src/common -I@top_srcdir@/src
mapedit_CXXFLAGS = -D_VERSION_=\"0.1\" $(GTK_CFLAGS) $(PY_CFLAGS) $(ADONTHELL_CFLAGS) ${IGE_MAC_CFLAGS}
mapedit_LDADD = ../common/libcommon.a $(GTK_LIBS) $(ADONTHELL_LIBS) $(PY_LIBS) ${IGE_MAC_LIBS}
