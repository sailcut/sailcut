# Macros for generating Sailcut CAD's icons

find_program(INKSCAPE inkscape)
find_program(PNGTOPNM pngtopnm)
find_program(PPMTOXPM ppmtoxpm)

macro(generate_xpm OUTPUT)
	set(${OUTPUT})
	foreach(_this_svg ${ARGN})
		get_filename_component(_svg_name ${_this_svg} NAME_WE)
		get_filename_component(_svg_path ${_this_svg} PATH)
		set(_this_xpm ${CMAKE_CURRENT_BINARY_DIR}/${_svg_path}${_svg_name}.xpm)
		set(_tmp_alpha ${_this_xpm}.tmp.alpha)
		set(_tmp_png ${_this_xpm}.tmp.png)
		add_custom_command(OUTPUT ${_this_xpm}
			DEPENDS ${_this_svg}
			COMMAND ${INKSCAPE} -z --file=${CMAKE_CURRENT_SOURCE_DIR}/${_this_svg} --export-png=${_tmp_png} --export-width=32 --export-height=32
			COMMAND ${PNGTOPNM} -alpha ${_tmp_png} > ${_tmp_alpha}
			COMMAND ${PNGTOPNM} ${_tmp_png} | ${PPMTOXPM} -alpha ${_tmp_alpha} -name ${_svg_name}_xpm | sed -e 's/static char/static const char/g' > ${_this_xpm}
			COMMAND rm -f ${_tmp_alpha} ${_tmp_png})
		list(APPEND ${OUTPUT} ${_this_xpm})
	endforeach()
endmacro(generate_xpm)

