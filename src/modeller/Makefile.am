## Process this file with automake to produce Makefile.in

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

INCLUDES = -I@top_srcdir@/src/common -I@top_srcdir@/src
modeller_CXXFLAGS = -D_VERSION_=\"0.1\" $(GTK_CFLAGS) $(PY_CFLAGS) $(ADONTHELL_CFLAGS) ${IGE_MAC_CFLAGS}
modeller_LDADD = ../common/libcommon.a $(GTK_LIBS) $(ADONTHELL_LIBS) ${IGE_MAC_LIBS}
