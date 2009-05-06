option(SHIP_DEPENDS_ENABLED "Whether to include dependencies in release packages")
find_package(PythonInterp)
set(SHIP_DEPENDS_FILE ${CMAKE_BINARY_DIR}/shipdepends.cmake)
set(SHIP_DEPENDS_SCRIPT ${CMAKE_SOURCE_DIR}/cmake/scripts/shipdepends.py)

# SHIP_DEPENDS(TARGETS)
macro(ship_depends)
	if(SHIP_DEPENDS_ENABLED)
		foreach(_this_target ${ARGN})
			get_target_property(_this_file ${_this_target} LOCATION)
			add_custom_command(TARGET ${_this_target} POST_BUILD
				COMMAND ${PYTHON_EXECUTABLE} ${SHIP_DEPENDS_SCRIPT} -c ${SHIP_DEPENDS_FILE} -x ${CMAKE_BINARY_DIR} ${_this_file}
				DEPENDS ${_this_target})
		endforeach()
	endif(SHIP_DEPENDS_ENABLED)
endmacro(ship_depends)

include(${SHIP_DEPENDS_FILE} OPTIONAL)
