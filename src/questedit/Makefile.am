## Process this file with automake to produce Makefile.in

EXTRA_DIST = questedit.glade

noinst_PROGRAMS = questedit

noinst_HEADERS = \
    gui_questedit.h \
    qed_cmdline.h
    
questedit_SOURCES = \
    gui_questedit.cc \
    main.cc \
    qed_cmdline.cc

# just for the dependency
gui_questedit.cc : questedit.glade.h

# generate a string out of the glade XML
$(srcdir)/questedit.glade.h : questedit.glade
	sed -e 's/"/\\"/g' -e 's/>$$/>"/' -e 's/^\( *\)</\1"</' $< > $@

INCLUDES = -I@top_srcdir@/src/common -I@top_srcdir@/src
questedit_CXXFLAGS = -D_VERSION_=\"0.1\" $(GTK_CFLAGS) $(PY_CFLAGS) $(ADONTHELL_CFLAGS) ${IGE_MAC_CFLAGS}
questedit_LDADD = ../common/libcommon.a $(GTK_LIBS) $(ADONTHELL_LIBS) ${IGE_MAC_LIBS}
