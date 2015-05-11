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

### Find libraries

# Find yaml-cpp
find_package(YamlCpp REQUIRED)
if(YAMLCPP_FOUND)
	include_directories(${YAMLCPP_INCLUDE_DIR})
	list(APPEND LIBS_SERIALIZATION "${YAMLCPP_LIBRARY}")
else()
	message(FATAL_ERROR "yaml-cpp is required.")
endif()

# Find boost
find_package(Boost COMPONENTS program_options REQUIRED)
if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	list(APPEND LIBS_SERIALIZATION "${Boost_LIBRARIES}")
else()
	message(FATAL_ERROR "Boost::program_options is required.")
endif()

# Find mosquittopp
find_package(Mosquittopp REQUIRED)
if(MOSQUITTOPP_FOUND)
	include_directories(${MOSQUITTOPP_INCLUDE_DIR})
	list(APPEND LIBS_COMMUNICATION "${MOSQUITTOPP_LIBRARY}")
else()
	message(FATAL_ERROR "mosquittopp is required.")
endif()

# Find mosquitto
find_package(Mosquitto REQUIRED)
if(MOSQUITTO_FOUND)
	include_directories(${MOSQUITTO_INCLUDE_DIR})
	list(APPEND LIBS_COMMUNICATION "${MOSQUITTO_LIBRARY}")
else()
	message(FATAL_ERROR "mosquitto is required.")
endif()

# Find libvirt
#TODO: Implementation of find_package module for libvirt
#list(APPEND LIBS "virt")

### Define source files.
set(SRC_COMMUNICATION ${PROJECT_SOURCE_DIR}/communication/mqtt_communicator.cpp)
set(SRC_SERIALIZATION ${PROJECT_SOURCE_DIR}/serialization/serializable.cpp)
set(SRC_EXAMPLE_COMMUNICATION ${PROJECT_SOURCE_DIR}/examples/communication/main.cpp)
set(SRC_EXAMPLE_COMM_SERIAL ${PROJECT_SOURCE_DIR}/examples/comm_serial/main.cpp)
include_directories(${PROJECT_SOURCE_DIR}/communication)
include_directories(${PROJECT_SOURCE_DIR}/serialization)

### Build and installation targets
# Add executable
add_library(fastlib_communication SHARED ${SRC_COMMUNICATION})
add_library(fastlib_serialization SHARED ${SRC_SERIALIZATION})

add_executable(fastlib_example_communication ${SRC_EXAMPLE_COMMUNICATION})
target_link_libraries(fastlib_example_communication ${LIBS_COMMUNICATION} fastlib_communication)

add_executable(fastlib_example_comm_serial ${SRC_EXAMPLE_COMM_SERIAL})
target_link_libraries(fastlib_example_comm_serial ${LIBS_COMMUNICATION} ${LIBS_SERIALIZATION} fastlib_communication fastlib_serialization)
