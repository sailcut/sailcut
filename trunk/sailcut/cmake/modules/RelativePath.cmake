# RELATIVE_PATH(OUTPUT BASE TARGET)
# Calculate the path to TARGET relative to BASE.
macro(relative_path OUTPUT BASE TARGET)
    string(REGEX REPLACE "[^/]+" ".." _temp ${BASE})
    set(${OUTPUT} "${_temp}/${TARGET}")
endmacro(relative_path)

