TMPFILE = sailcut.tmp.png

GENPIXMAPS = sailcut.png sailcut-file.png

pixmapsdir = $(prefix)/share/pixmaps
dist_pixmaps_DATA = sailcut.svg sailcut-file.svg sailcut.xpm sailcut.ico $(GENPIXMAPS)

sailcut.png: sailcut.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=64 --export-height=64

sailcut-file.png: sailcut-file.svg
	$(SODIPODI) -z --file=$< --export-png=$@ --export-width=64 --export-height=64

sailcut.xpm: sailcut.svg
	$(SODIPODI) -z --file=$< --export-png=$(TMPFILE) --export-width=32 --export-height=32
	if [ -f $(TMPFILE) ]; then \
	  $(PNGTOPNM) $(TMPFILE) | $(PPMTOXPM) -name sailcut_xpm > $@; \
	  sed -i -e 's/\"   c #000000\"/\"   c None\"/' $@; \
	  sed -i -e 's/static char/static const char/' $@; \
	  rm -f $(TMPFILE); \
	fi

EXTRA_DIST = sailcut-file-alt.svg sailcut.rc resource.h $(dist_pixmaps_DATA)
MAINTAINERCLEANFILES = $(GENPIXMAPS)
