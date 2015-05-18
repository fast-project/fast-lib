#
# This file is part of fast-lib.
# Copyright (C) 2015 RWTH Aachen University - ACS
#
# This file is licensed under the GNU Lesser General Public License Version 3
# Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
#

cmake_minimum_required(VERSION 2.6)
project(fast-lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Define version number
set(FASTLIB_VERSION_MAJOR "0")
set(FASTLIB_VERSION_MINOR "1")
set(FASTLIB_VERSION_PATCH "0")
set(FASTLIB_VERSION "${FASTLIB_VERSION_MAJOR}.${FASTLIB_VERSION_MINOR}.${FASTLIB_VERSION_PATCH}")

#### Set compiler flags
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" CXX11_SUPPORTED)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" CXX0X_SUPPORTED)
if(CXX11_SUPPORTED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(CXX0X_SUPPORTED)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support.")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(BUILD_SHARED_LIBS ON CACHE BOOL "If ON build shared, else static libraries.")

### Find libraries

# Find yaml-cpp
if(NOT BUILD_SHARED_LIBS)
	set(YAMLCPP_STATIC_LIBRARY ON)
endif()
find_package(YamlCpp REQUIRED)
if(YAMLCPP_FOUND)
	include_directories(SYSTEM ${YAMLCPP_INCLUDE_DIR})
	list(APPEND LIBS_SERIALIZATION "${YAMLCPP_LIBRARY}")
else()
	message(FATAL_ERROR "yaml-cpp is required.")
endif()

# Find mosquittopp
find_package(Mosquittopp REQUIRED)
if(MOSQUITTOPP_FOUND)
	include_directories(SYSTEM ${MOSQUITTOPP_INCLUDE_DIR})
	list(APPEND LIBS_COMMUNICATION "${MOSQUITTOPP_LIBRARY}")
else()
	message(FATAL_ERROR "mosquittopp is required.")
endif()

# Find mosquitto
find_package(Mosquitto REQUIRED)
if(MOSQUITTO_FOUND)
	include_directories(SYSTEM ${MOSQUITTO_INCLUDE_DIR})
	list(APPEND LIBS_COMMUNICATION "${MOSQUITTO_LIBRARY}")
else()
	message(FATAL_ERROR "mosquitto is required.")
endif()

### Define source files.
set(SRC_COMMUNICATION ${PROJECT_SOURCE_DIR}/communication/mqtt_communicator.cpp)
set(SRC_SERIALIZATION ${PROJECT_SOURCE_DIR}/serialization/serializable.cpp)
set(SRC_EXAMPLE_COMMUNICATION ${PROJECT_SOURCE_DIR}/examples/communication/main.cpp)
set(SRC_EXAMPLE_COMM_SERIAL ${PROJECT_SOURCE_DIR}/examples/comm_serial/main.cpp)
include_directories(${PROJECT_SOURCE_DIR}/communication)
include_directories(${PROJECT_SOURCE_DIR}/serialization)

### Build and installation targets
# Add executable
add_library(fastlib_communication ${SRC_COMMUNICATION})
target_link_libraries(fastlib_communication ${LIBS_COMMUNICATION})

add_library(fastlib_serialization ${SRC_SERIALIZATION})
target_link_libraries(fastlib_serialization ${LIBS_SERIALIZATION})

add_executable(fastlib_example_communication ${SRC_EXAMPLE_COMMUNICATION})
target_link_libraries(fastlib_example_communication fastlib_communication)

add_executable(fastlib_example_comm_serial ${SRC_EXAMPLE_COMM_SERIAL})
target_link_libraries(fastlib_example_comm_serial fastlib_communication fastlib_serialization)
