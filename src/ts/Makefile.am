DIST_TRANSLATIONS = sailcut.de.ts sailcut.fr.ts sailcut.it.ts sailcut.nl.ts sailcut.no.ts sailcut.pt.ts sailcut.dk.ts sailcut.ru.ts
TRANSLATIONS = $(DIST_TRANSLATIONS) sailcut.template.ts

pkgdata_DATA = $(DIST_TRANSLATIONS:.ts=.qm)

%.qm: %.ts
	$(LRELEASE) $< -qm $@

EXTRA_DIST = $(TRANSLATIONS)
CLEANFILES = $(pkgdata_DATA)
