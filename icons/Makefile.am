include $(top_srcdir)/Makefile.common

ICONS_MACOS = sailcut.icns
ICONS_WIN32 = sailcut.ico sailcut-file.ico

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

dist_noinst_DATA = sailcut.xpm panel.xpm panelgroup.xpm point.xpm

EXTRA_DIST = $(ICONS_MAC) $(ICONS_WIN32) sailcut.svg sailcut-file.svg sailcut-file-alt.svg
MAINTAINERCLEANFILES = $(ICONS_WIN32) $(dist_noinst_DATA)

