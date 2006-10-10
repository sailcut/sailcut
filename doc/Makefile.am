SUBDIRS = en es fr

htmldir = $(pkgdocdir)/html
dist_html_DATA = sailcut-handbook.css

# this makes the handbook's stylesheet available
# when compiling outside the source directory
out_of_source = $(filter-out .,$(srcdir))

sailcut-handbook.css: FORCE
	$(if $(out_of_source),cp $(srcdir)/sailcut-handbook.css $(CURDIR))

.PHONY: FORCE
FORCE:

CLEANFILES = $(if $(out_of_source),sailcut-handbook.css)
EXTRA_DIST = Makefile.docbook sailcut-html.xsl
