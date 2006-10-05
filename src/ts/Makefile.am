DIST_TRANSLATIONS = sailcut.de.ts sailcut.fr.ts sailcut.it.ts sailcut.nl.ts sailcut.no.ts sailcut.pt.ts sailcut.dk.ts sailcut.se.ts sailcut.fi.ts sailcut.es.ts sailcut.se.ts sailcut.ru.ts
TRANSLATIONS = $(DIST_TRANSLATIONS) sailcut.template.ts
# TODO : sailcut.es.ts sailcut.se.ts sailcut.fi.ts sailcut.ru.ts

SOURCEFILES = $(addprefix $(top_srcdir)/src/,*.cpp *.h *.ui)

pkgdata_DATA = $(DIST_TRANSLATIONS:.ts=.qm)

%.qm: %.ts
	$(LRELEASE) $< -qm $@

lupdate:
	$(LUPDATE) $(SOURCEFILES) -ts $(addprefix $(srcdir)/, $(TRANSLATIONS))

EXTRA_DIST = $(TRANSLATIONS)
CLEANFILES = $(pkgdata_DATA)
