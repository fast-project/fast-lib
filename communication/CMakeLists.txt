#
# This file is part of fast-lib.
# Copyright (C) 2015 RWTH Aachen University - ACS
#
# This file is licensed under the GNU Lesser General Public License Version 3
# Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
#

### Find libraries
# Find mosquittopp
find_package(Mosquittopp REQUIRED)
if(MOSQUITTOPP_FOUND)
	include_directories(SYSTEM ${MOSQUITTOPP_INCLUDE_DIR})
	list(APPEND LIBS "${MOSQUITTOPP_LIBRARY}")
else()
	message(FATAL_ERROR "mosquittopp is required.")
endif()

# Find mosquitto
find_package(Mosquitto REQUIRED)
if(MOSQUITTO_FOUND)
	include_directories(SYSTEM ${MOSQUITTO_INCLUDE_DIR})
	list(APPEND LIBS "${MOSQUITTO_LIBRARY}")
else()
	message(FATAL_ERROR "mosquitto is required.")
endif()

### Define source files.
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_communicator.cpp)

### Build and installation targets
# Add library
add_library(${FASTLIB_COMMUNICATION} ${SRC})
target_link_libraries(${FASTLIB_COMMUNICATION} ${LIBS})
