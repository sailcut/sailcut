DIST_TRANSLATIONS = sailcut_de.ts sailcut_fr.ts sailcut_it.ts sailcut_nl.ts sailcut_no.ts sailcut_pt.ts sailcut_dk.ts sailcut_ru.ts
TRANSLATIONS = $(DIST_TRANSLATIONS) template.ts

pkgdata_DATA = $(DIST_TRANSLATIONS:.ts=.qm)

%.qm: %.ts
	$(LRELEASE) $< -qm $@

EXTRA_DIST = $(TRANSLATIONS)
CLEANFILES = $(pkgdata_DATA)
