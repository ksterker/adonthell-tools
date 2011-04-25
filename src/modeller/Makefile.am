## Process this file with automake to produce Makefile.in

EXTRA_DIST = modeller.glade

noinst_PROGRAMS = modeller

noinst_HEADERS = \
    gui_modeller.h \
    gui_preview.h \
    mdl_cmdline.h \
    mdl_handle.h \
    mdl_renderer.h
    
modeller_SOURCES = \
    gui_modeller.cc \
    gui_preview.cc \
    main.cc \
    mdl_cmdline.cc \
    mdl_renderer.cc

# just for the dependency
gui_modeller.cc : modeller.glade.h

# generate a string out of the glade XML
$(srcdir)/modeller.glade.h : modeller.glade
	$(SED) -e 's/"/\\"/g' -e 's/>$$/>"/' -e 's/^\( *\)</\1"</' $< > $@

# preparation for switching to GTK+ 3.0
GTK_3_0_FLAGS = -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE

INCLUDES = -I@top_srcdir@/src/common -I@top_srcdir@/src
modeller_CXXFLAGS = -D_VERSION_=\"0.1\" $(GTK_CFLAGS) ${GTK_3_0_FLAGS} $(PY_CFLAGS) $(ADONTHELL_CFLAGS) ${IGE_MAC_CFLAGS}
modeller_LDADD = ../common/libcommon.a $(GTK_LIBS) $(ADONTHELL_LIBS) ${IGE_MAC_LIBS}
