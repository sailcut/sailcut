pixmapsdir = $(prefix)/share/pixmaps
dist_pixmaps_DATA = sailcut.xpm panel.xpm panelgroup.xpm point.xpm

dist_noinst_DATA = sailcut.ico sailcut-file.ico

%.ico: %-48x48.png %-32x32.png %-16x16.png
	$(ICOTOOL) -c -o $@ $+

%-16x16.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=16 --export-height=16

%-32x32.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=32 --export-height=32

%-48x48.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=48 --export-height=48

$(dist_pixmaps_DATA): %.xpm: %-32x32.png
	$(PNGTOPNM) -alpha $< > $<.alpha
	$(PNGTOPNM) $< | $(PPMTOXPM) -alpha $<.alpha -name $*_xpm > $@.tmp
	rm -f $<.alpha
	mv -f $@.tmp $@

EXTRA_DIST = sailcut.svg sailcut-file.svg sailcut-file-alt.svg
MAINTAINERCLEANFILES = $(dist_noinst_DATA)
