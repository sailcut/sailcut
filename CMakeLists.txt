cmake_minimum_required(VERSION 2.6)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include(RelativePath)
include(SailcutDocs)
include(ShipDepends)

# project and version
project(sailcut)
set(SAILCUT_VERSION_MAJOR 1)
set(SAILCUT_VERSION_MINOR 3)
set(SAILCUT_VERSION_PATCH 5)
set(SAILCUT_VERSION ${SAILCUT_VERSION_MAJOR}.${SAILCUT_VERSION_MINOR}.${SAILCUT_VERSION_PATCH})

# dependencies
find_package(Qt4 REQUIRED)
# FIXME: when cross-compiling for mingw32 FindQt4 picks up
# the host's dependencies for QtOpenGl (-lGLU -GL)
if(WIN32)
set(QT_QTOPENGL_LIB_DEPENDENCIES -lopengl32 -lglu32)
endif(WIN32)

# configuration
set(SAILCUT_RUNTIME_DIR bin CACHE STRING "Sailcut runtime directory.")
set(SAILCUT_LIBRARY_DIR lib CACHE STRING "Sailcut library directory.")
set(SAILCUT_DATA_DIR share/${PROJECT_NAME} CACHE STRING "Sailcut data directory.")
set(SAILCUT_DOC_DIR share/doc/${PROJECT_NAME}/html CACHE STRING "Sailcut documentation directory.")

# packaging
set(CPACK_PACKAGE_CONTACT "info@sailcut.com")
set(CPACK_PACKAGE_DESCRIPTION "Sailcut CAD is a sail design and plotting software")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Sailcut CAD ${SAILCUT_VERSION}")
set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "Sailcut CAD ${SAILCUT_VERSION}")
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/COPYING)
set(CPACK_PACKAGE_EXECUTABLES "sailcut" "Sailcut CAD")
set(CPACK_PACKAGE_VERSION_MAJOR ${SAILCUT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SAILCUT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SAILCUT_VERSION_PATCH})
if(APPLE)
	set(CPACK_BUNDLE_NAME ${PROJECT_NAME})
	set(CPACK_BUNDLE_ICON ${CMAKE_SOURCE_DIR}/icons/sailcut.icns)
	set(CPACK_BUNDLE_IDENTIFIER com.sailcut.Sailcut)
	set(CPACK_BUNDLE_PLIST ${CMAKE_BINARY_DIR}/Bundle.plist)
	set(CPACK_BUNDLE_STARTUP_COMMAND ${CMAKE_BINARY_DIR}/Bundle.start)
endif(APPLE)
include(CPack)
configure_file(${CMAKE_SOURCE_DIR}/cmake/Bundle.plist.in ${CMAKE_BINARY_DIR}/Bundle.plist)
configure_file(${CMAKE_SOURCE_DIR}/cmake/Bundle.start.in ${CMAKE_BINARY_DIR}/Bundle.start)

add_subdirectory(doc)
add_subdirectory(icons)
add_subdirectory(src)
