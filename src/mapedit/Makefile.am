## Process this file with automake to produce Makefile.in

EXTRA_DIST = \
    entity-properties.glade \
    grid-properties.glade \
    zone-properties.glade

noinst_PROGRAMS = mapedit

noinst_HEADERS = \
    entity-properties.glade.h \
    grid-properties.glade.h \
    gui_entity_dialog.h \
    gui_entity_list.h \
    gui_grid.h \
    gui_grid_dialog.h \
    gui_mapedit.h \
    gui_mapedit_events.h \
    gui_mapview.h \
    gui_mapview_events.h \
    gui_script_selector.h \
    gui_renderheight.h \
    gui_zone.h \
    gui_zone_dialog.h \
    gui_zone_list.h \
    map_cmdline.h \
    map_data.h \
    map_entity.h \
    map_mgr.h \
    map_renderer.h \
    zone-properties.glade.h
    
mapedit_SOURCES = \
    gui_entity_dialog.cc \
    gui_entity_list.cc \
    gui_grid.cc \
    gui_grid_dialog.cc \
    gui_mapedit.cc \
    gui_mapedit_events.cc \
    gui_mapview.cc \
    gui_mapview_events.cc \
    gui_script_selector.cc \
    gui_renderheight.cc \
    gui_zone.cc \
    gui_zone_dialog.cc \
    gui_zone_list.cc \
    main.cc \
    map_cmdline.cc \
    map_data.cc \
    map_entity.cc \
    map_renderer.cc

# just for the dependency
gui_entity_dialog.cc : entity-properties.glade.h
gui_grid_dialog.cc : grid-properties.glade.h
gui_zone_dialog.cc : zone-properties.glade.h

# generate a string out of the glade XML
%.glade.h : %.glade
	$(SED) -e 's/"/\\"/g' -e 's/>$$/>"/' -e 's/^\( *\)</\1"</' $< > $(srcdir)/$@

INCLUDES = -I@top_srcdir@/src/common -I@top_srcdir@/src
mapedit_CXXFLAGS = -D_VERSION_=\"0.1\" $(GTK_CFLAGS) $(PY_CFLAGS) $(ADONTHELL_CFLAGS) ${IGE_MAC_CFLAGS}
mapedit_LDADD = ../common/libcommon.a $(GTK_LIBS) $(ADONTHELL_LIBS) $(PY_LIBS) ${IGE_MAC_LIBS}
