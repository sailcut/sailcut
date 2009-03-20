qt4_add_translation(QMS 
	sailcut_de.ts
	sailcut_dk.ts
	sailcut_fr.ts
	sailcut_it.ts
	sailcut_nl.ts
	sailcut_no.ts
	sailcut_pt.ts
	sailcut_ru.ts)
add_custom_target(translations DEPENDS ${QMS})

install(FILES ${QMS} DESTINATION ${SAILCUT_DATA_DIR})

