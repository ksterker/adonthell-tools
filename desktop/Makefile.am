## Process this file with automake to produce Makefile.in

## -- .desktop files

desktopdir = $(datadir)/applications
desktop_DATA = dlgedit.desktop


## -- mime types

mimedir = $(datadir)/mime
mimepackagesdir = $(mimedir)/packages
dist_mimepackages_DATA = dlgedit.xml


## -- make install

install-data-hook:
if UPDATE_MIME
	test -n "$(DESTDIR)" || $(UPDATE_MIME_DATABASE) "$(datadir)/mime"
endif

if UPDATE_DESKTOP
	test -n "$(DESTDIR)" || $(UPDATE_DESKTOP_DATABASE)
endif


## -- make uninstall

uninstall-hook: 
if UPDATE_MIME
	$(UPDATE_MIME_DATABASE) "$(datadir)/mime"
endif

