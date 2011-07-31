## Process this file with automake to produce Makefile.in

## -- .desktop files

desktopdir = $(datadir)/applications
dist_desktop_DATA = dlgedit.desktop mapedit.desktop modeller.desktop


## -- mime types

mimedir = $(datadir)/mime
mimepackagesdir = $(mimedir)/packages
dist_mimepackages_DATA = dlgedit.xml mapedit.xml modeller.xml


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

