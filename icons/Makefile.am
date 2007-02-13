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

%.ico: %.svg
	for i in 48 32 16; do \
		$(INKSCAPE) -z --file=$< --export-png=$*.tmp.$$i --export-width=$$i --export-height=$$i || exit 1; \
	done
	$(ICOTOOL) -c -o $@ $*.tmp.48 $*.tmp.32 $*.tmp.16
	rm -f $*.tmp.*

%.xpm: %.svg
	$(INKSCAPE) -z --file=$< --export-png=$*.tmp.png --export-width=32 --export-height=32
	$(PNGTOPNM) -alpha $*.tmp.png > $*.tmp.alpha
	$(PNGTOPNM) $*.tmp.png | $(PPMTOXPM) -alpha $*.tmp.alpha -name $*_xpm > $@
	rm -f $*.tmp.*

EXTRA_DIST = $(ICONS_MAC) $(ICONS_WIN32) sailcut.svg sailcut-file.svg sailcut-file-alt.svg
MAINTAINERCLEANFILES = $(ICONS_WIN32) $(dist_noinst_DATA)

