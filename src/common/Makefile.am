## Process this file with automake to produce Makefile.in

noinst_LIBRARIES = libcommon.a

noinst_HEADERS = \
    gui_file.h \
    gui_modal_dialog.h \
    gui_recent_files.h \
    gui_scrollable.h \
    util.h

libcommon_a_SOURCES = \
    gui_file.cc \
    gui_modal_dialog.cc \
    gui_recent_files.cc \
    gui_scrollable.cc \
    util.cc

# preparation for switching to GTK+ 3.0
GTK_3_0_FLAGS = -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE

libcommon_a_CXXFLAGS = $(ADONTHELL_CFLAGS) $(GTK_CFLAGS) $(GTK_3_0_FLAGS)
