AM_CXXFLAGS = -I$(top_srcdir)/src

noinst_PROGRAMS = backendtest

backendtest_CXXFLAGS = $(CAIRO_CFLAGS) $(GTK_CFLAGS) $(ADONTHELL_CFLAGS) $(PY_CFLAGS) $(AM_CXXFLAGS) 
backendtest_SOURCES = backendtest.cc
backendtest_LDADD = $(CAIRO_LIBS) $(GTK_LIBS) $(ADONTHELL_LIBS) $(PY_LIBS)
