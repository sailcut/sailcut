# Macros for generating Sailcut CAD's documentation

find_program(FIG2DEV fig2dev)
find_program(XSLTPROC xsltproc)

# TRANSFORM_DOCBOOK(OUTPUT INPUT..)
macro(transform_docbook OUTPUT)

endmacro(transform_docbook)

# TRANSFORM_FIGS(OUTPUT INPUT..)
macro(transform_figs OUTPUT)
	set(${OUTPUT})
	foreach(THIS_FIG ${ARGN})
		get_filename_component(_fig_name ${THIS_FIG} NAME_WE)
		get_filename_component(_fig_path ${THIS_FIG} PATH)
		set(THIS_PNG ${_fig_path}/${_fig_name}.png)
		add_custom_command(OUTPUT ${THIS_PNG}
			COMMAND ${FIG2DEV} -L png -S2 ${CMAKE_CURRENT_SOURCE_DIR}/${THIS_FIG} ${THIS_PNG}
			DEPENDS ${THIS_FIG})
		list(APPEND ${OUTPUT} ${THIS_PNG})
	endforeach()
endmacro(transform_figs)

