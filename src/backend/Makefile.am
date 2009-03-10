pkglibgfxdir = $(ADONTHELL_BACKEND)/adonthell/gfx
pkglibgfx_LTLIBRARIES = _gtk.la

### GTK backend

## Our header files
noinst_HEADERS = \
	gtk/screen_gtk.h \
	gtk/surface_gtk.h

## Rules to build _gtk.so
_gtk_la_SOURCES = \
	gtk/gfx_gtk.cc \
	gtk/screen_gtk.cc \
	gtk/surface_gtk.cc

_gtk_la_CXXFLAGS = $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(ADONTHELL_CFLAGS) $(AM_CXXFLAGS)
_gtk_la_LDFLAGS = -module -avoid-version -no-undefined
_gtk_la_LIBADD = $(CAIRO_LIBS) $(GTK_LIBS) $(ADONTHELL_LIBS)
