include_directories(${CMAKE_SOURCE_DIR}/src)

add_library(geocpp STATIC
	geometry.cpp
	matrix.cpp
	subspace.cpp)

add_executable(geotest geotest.cpp)
target_link_libraries(geotest geocpp)
