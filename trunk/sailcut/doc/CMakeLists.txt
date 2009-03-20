set(docfiles)

if(XSLTPROC AND FIG2DEV)
foreach(language en es fr)
	file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${language})
	transform_figs(pngs
		${language}/coordinates_system.fig
		${language}/sailplan.fig
		${language}/sail_seams.fig
		${language}/sail_edges.fig
		${language}/develop_panel_drawing.fig
		${language}/head_gores_definition.fig
		${language}/rigplan.fig)
	transform_docbook(html sailcut-html.xsl ${language}/index.docbook)
	list(APPEND docfiles ${html} ${pngs})
	install(FILES ${docfiles} DESTINATION ${SAILCUT_DOC_DIR}/html/${language})
endforeach()
install(FILES sailcut-handbook.css DESTINATION ${SAILCUT_DOC_DIR}/html)
else(XSLTPROC AND FIG2DEV)
message(STATUS "Not generating documentation")
endif(XSLTPROC AND FIG2DEV)

add_custom_target(documentation DEPENDS ${docfiles})

