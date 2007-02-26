include $(top_srcdir)/Makefile.common

ICONS_MACOS = sailcut.icns
ICONS_WIN32 = sailcut.ico sailcut-file.ico
ICONS_XPM = sailcut.xpm panel.xpm panelgroup.xpm point.xpm

if MACOS
ICONS = $(ICONS_MACOS)
else
if WIN32
ICONS = $(ICONS_WIN32)
else
ICONS = sailcut.xpm
endif
endif

dist_pkgicon_DATA = $(ICONS)
dist_noinst_DATA = $(ICONS_XPM)

EXTRA_DIST = $(ICONS_MAC) $(ICONS_WIN32) $(ICONS_XPM:.xpm=.svg)
MAINTAINERCLEANFILES = $(ICONS_WIN32) $(ICONS_XPM)

