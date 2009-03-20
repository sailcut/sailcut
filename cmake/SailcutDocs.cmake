# Macros for generating Sailcut CAD's documentation

find_program(FIG2DEV fig2dev)
find_program(XSLTPROC xsltproc)

# TRANSFORM_DOCBOOK(OUTPUT STYLESHEET DOCBOOK)
macro(transform_docbook OUTPUT STYLESHEET INPUT)
	get_filename_component(_docbook_name ${INPUT} NAME_WE)
	get_filename_component(_docbook_path ${INPUT} PATH)
	set(${OUTPUT} ${_docbook_path}/${_docbook_name}.html)
	add_custom_command(OUTPUT ${${OUTPUT}}
		COMMAND ${XSLTPROC} -o ${${OUTPUT}} ${CMAKE_CURRENT_SOURCE_DIR}/${STYLESHEET} ${CMAKE_CURRENT_SOURCE_DIR}/${INPUT}
		DEPENDS ${STYLESHEET} ${INPUT})
endmacro(transform_docbook)

# TRANSFORM_FIGS(OUTPUT FIG..)
macro(transform_figs OUTPUT)
	set(${OUTPUT})
	foreach(_this_fig ${ARGN})
		get_filename_component(_fig_name ${_this_fig} NAME_WE)
		get_filename_component(_fig_path ${_this_fig} PATH)
		set(_this_png ${_fig_path}/${_fig_name}.png)
		add_custom_command(OUTPUT ${_this_png}
			COMMAND ${FIG2DEV} -L png -S2 ${CMAKE_CURRENT_SOURCE_DIR}/${_this_fig} ${_this_png}
			DEPENDS ${_this_fig})
		list(APPEND ${OUTPUT} ${_this_png})
	endforeach()
endmacro(transform_figs)

