pixmapsdir = $(prefix)/share/pixmaps
dist_pixmaps_DATA = sailcut.xpm panel.xpm panelgroup.xpm point.xpm

dist_noinst_DATA = sailcut.ico sailcut-file.ico

%.ico: %.svg
	for i in 48 32 16; do \
		$(SODIPODI) -z --file=$< --export-png=$*.tmp.$$i --export-width=$$i --export-height=$$i || exit 1; \
	done
	$(ICOTOOL) -c -o $@ $*.tmp.48 $*.tmp.32 $*.tmp.16
	rm -f $*.tmp.*

$(dist_pixmaps_DATA): %.xpm: %.svg
	$(SODIPODI) -z --file=$< --export-png=$*.tmp.png --export-width=32 --export-height=32
	$(PNGTOPNM) -alpha $*.tmp.png > $*.tmp.alpha
	$(PNGTOPNM) $*.tmp.png | $(PPMTOXPM) -alpha $*.tmp.alpha -name $*_xpm > $@
	rm -f $*.tmp.*

EXTRA_DIST = sailcut.icns sailcut.svg sailcut-file.svg sailcut-file-alt.svg
MAINTAINERCLEANFILES = $(dist_pixmaps_DATA) $(dist_noinst_DATA)
