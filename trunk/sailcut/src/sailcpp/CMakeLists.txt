include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(sailcpp STATIC
	boatdef.cpp
	hulldef.cpp
	hullworker.cpp
	panel.cpp
	panelgroup.cpp
	rigdef.cpp
	rigworker.cpp
	sailcalc.cpp
	saildef.cpp
	sailmould.cpp
	sailworker.cpp)

add_executable(sailtest sailtest.cpp)
target_link_libraries(sailtest sailcpp geocpp)

