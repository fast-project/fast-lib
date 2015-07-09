#
# This file is part of fast-lib.
# Copyright (C) 2015 RWTH Aachen University - ACS
#
# This file is licensed under the GNU Lesser General Public License Version 3
# Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
#

### Find libraries
# Find yaml-cpp
if(NOT BUILD_SHARED_LIBS)
	set(YAMLCPP_STATIC_LIBRARY ON)
endif()
find_package(YamlCpp REQUIRED)
if(YAMLCPP_FOUND)
	include_directories(SYSTEM ${YAMLCPP_INCLUDE_DIR})
	list(APPEND LIBS "${YAMLCPP_LIBRARY}")
else()
	message(FATAL_ERROR "yaml-cpp is required.")
endif()

### Define source files.
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/serializable.cpp)
set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/serializable.hpp)

### Build and installation targets
# Add library
add_library(${FASTLIB_SERIALIZATION} ${SRC})
target_link_libraries(${FASTLIB_SERIALIZATION} ${LIBS})
install(TARGETS ${FASTLIB_SERIALIZATION}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
install(FILES ${HEADERS}
	DESTINATION include/fast-lib/serialization)
