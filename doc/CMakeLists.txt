set(docfiles)
foreach(language en fr)
	transform_figs(pngs
		${language}/coordinates_system.fig
		${language}/sailplan.fig
		${language}/sail_seams.fig
		${language}/sail_edges.fig
		${language}/develop_panel_drawing.fig
		${language}/head_gores_definition.fig
		${language}/rigplan.fig)
	list(APPEND docfiles ${pngs})
endforeach()
add_custom_target(documentation DEPENDS ${docfiles})

