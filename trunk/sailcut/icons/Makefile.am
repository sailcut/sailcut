pixmapsdir = $(prefix)/share/pixmaps
dist_pixmaps_DATA = sailcut.xpm

dist_noinst_DATA = sailcut.ico sailcut-file.ico

%.ico: %-48x48.png %-32x32.png %-16x16.png
	$(ICOTOOL) -c -o $@ $+

%-16x16.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=16 --export-height=16

%-32x32.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=32 --export-height=32

%-48x48.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=48 --export-height=48

sailcut-new.xpm: sailcut-32x32.png
	$(PNGTOPNM) $< | $(PPMTOXPM) -name sailcut_xpm > $@.tmp
	sed -i -e 's/\"   c #000000\"/\"   c None\"/' $@.tmp
	sed -i -e 's/static char/static const char/' $@.tmp
	mv -f $@.tmp $@

EXTRA_DIST = sailcut.svg sailcut-file.svg sailcut-file-alt.svg
MAINTAINERCLEANFILES = $(dist_noinst_DATA)
