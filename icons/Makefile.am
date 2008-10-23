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

%.ico: %.svg
	for i in 48 32 16; do \
		$(INKSCAPE) -z --file=$< --export-png=$*.tmp.$$i --export-width=$$i --export-height=$$i || exit 1; \
	done
	$(ICOTOOL) -c -o $@ $*.tmp.48 $*.tmp.32 $*.tmp.16
	rm -f $*.tmp.*

%.xpm: %.svg
	if test -z "$(INKSCAPE)" -o -z "$(PNGTOPNM)"; then \
		echo "Please install inkscape and netpbm"; \
		exit 1; \
	fi
	$(INKSCAPE) -z --file=$< --export-png=$*.tmp.png --export-width=32 --export-height=32
	$(PNGTOPNM) -alpha $*.tmp.png > $*.tmp.alpha
	$(PNGTOPNM) $*.tmp.png | $(PPMTOXPM) -alpha $*.tmp.alpha -name $*_xpm | sed -e 's/static char/static const char/g' > $@
	rm -f $*.tmp.*

EXTRA_DIST = $(ICONS_MACOS) $(ICONS_WIN32) $(ICONS_XPM:.xpm=.svg)
MAINTAINERCLEANFILES = $(ICONS_WIN32) $(ICONS_XPM)

