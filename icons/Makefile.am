pixmapsdir = $(prefix)/share/pixmaps
dist_pixmaps_DATA = sailcut.xpm

dist_noinst_DATA = sailcut-32x32.ico sailcut-file-32x32.ico

%.ico: %.png
	$(PNGTOICO) $@ $<

%-32x32.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=32 --export-height=32

%-64x64.png: %.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=32 --export-height=32

sailcut.xpm: sailcut-32x32.png
	$(PNGTOPNM) $< | $(PPMTOXPM) -name sailcut_xpm > $@.tmp
	sed -i -e 's/\"   c #000000\"/\"   c None\"/' $@.tmp
	sed -i -e 's/static char/static const char/' $@.tmp
	mv -f $@.tmp $@

EXTRA_DIST = sailcut.svg sailcut-file.svg sailcut-file-alt.svg
MAINTAINERCLEANFILES = $(dist_pixmaps_DATA) $(dist_noinst_DATA)
