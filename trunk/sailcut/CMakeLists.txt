cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(SailcutDocs)

project(sailcut)
set(SAILCUT_VERSION_MAJOR 1)
set(SAILCUT_VERSION_MINOR 3)
set(SAILCUT_VERSION_PATCH 4)
set(SAILCUT_VERSION ${SAILCUT_VERSION_MAJOR}.${SAILCUT_VERSION_MINOR}.${SAILCUT_VERSION_PATCH})

# dependencies
find_package(Qt4 REQUIRED)

# configuration
set(SAILCUT_DATA_DIR share/${PROJECT_NAME})
set(SAILCUT_DOC_DIR share/doc/${PROJECT_NAME})

add_subdirectory(doc)
add_subdirectory(src)
