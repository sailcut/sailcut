# Macros for generating Sailcut CAD's documentation

# TRANSFORM_DOCBOOK(OUTPUT INPUT..)
macro(transform_docbook OUTPUT)

endmacro(transform_docbook)

# TRANSFORM_FIGS(OUTPUT INPUT..)
macro(transform_figs OUTPUT)
	set(${OUTPUT})
	foreach(THIS_FIG ${ARGN})
		get_filename_component(_basename ${THIS_FIG} NAME_WE)
		set(THIS_PNG ${_basename}.png)
		add_custom_command(OUTPUT ${THIS_PNG}
			COMMAND ${FIG2DEV} -L png -S2 ${CMAKE_CURRENT_SOURCE_DIR}/${THIS_FIG} ${THIS_PNG}
			DEPENDS ${THIS_FIG})
		list(APPEND ${OUTPUT} ${THIS_PNG})
	endforeach()
endmacro(transform_figs)

