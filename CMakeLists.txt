#
# This file is part of fast-lib.
# Copyright (C) 2015 RWTH Aachen University - ACS
#
# This file is licensed under the GNU Lesser General Public License Version 3
# Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
#

# Default build type: Debug
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type")
cmake_minimum_required(VERSION 2.8)
project(fast-lib)
if(POLICY CMP0026)
	cmake_policy(SET CMP0026 OLD) # Used to get location of boost libs
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Cache
set(BUILD_SHARED_LIBS OFF CACHE BOOL "If ON build shared, else static libraries.")
if(BUILD_SHARED_LIBS)
	message(FATAL_ERROR "BUILD_SHARED_LIBS=ON is not supported yet. Use static libs instead.")
endif()

set(ENABLE_LOGGING ON CACHE BOOL "Enable logging with boost.log.")
if(ENABLE_LOGGING)
	add_definitions(-DFASTLIB_ENABLE_LOGGING)
endif()

set(BUILD_EXAMPLES ON CACHE BOOL "Enable build of examples.")

# Library names
set(FASTLIB_COMMUNICATION "fastlib_communication")
set(FASTLIB_SERIALIZATION "fastlib_serialization")

# Define version number
set(FASTLIB_VERSION_MAJOR "0")
set(FASTLIB_VERSION_MINOR "1")
set(FASTLIB_VERSION_PATCH "0")
set(FASTLIB_VERSION "${FASTLIB_VERSION_MAJOR}.${FASTLIB_VERSION_MINOR}.${FASTLIB_VERSION_PATCH}")

# Set compiler flags
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

# Enable support for external projects
include(ExternalProject)

########
# mosquitto
ExternalProject_Add(mosquitto
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/mosquitto-1.3.5
	CMAKE_ARGS -DWITH_SRV=OFF -DWITH_TLS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_C_FLAGS=-fpic -DCMAKE_CXX_FLAGS=-fpic)

# Store mosquitto variables
ExternalProject_Get_Property(mosquitto install_dir)
set(MOSQUITTO_LIBRARIES ${install_dir}/lib/libmosquittopp.a ${install_dir}/lib/libmosquitto.a)
set(MOSQUITTO_INCLUDE_DIR "${install_dir}/include")

# Install headers
install(DIRECTORY "${MOSQUITTO_INCLUDE_DIR}/" DESTINATION include/fast-lib/external)
########

########
# Boost
include(add_boost)
include_directories(SYSTEM ${BoostSourceDir})

# Install headers
install(DIRECTORY "${BoostSourceDir}/boost" DESTINATION include/fast-lib/external)
########

########
# yaml-cpp
ExternalProject_Add(yaml-cpp
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/yaml-cpp
	CMAKE_ARGS -DCMAKE_CXX_FLAGS=-fpic -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>)

# Store yaml-cpp variables
ExternalProject_Get_Property(yaml-cpp install_dir)
set(YAMLCPP_LIBRARY "${install_dir}/lib/libyaml-cpp.a")
set(YAMLCPP_INCLUDE_DIR "${install_dir}/include")

# Install headers
install(DIRECTORY "${YAMLCPP_INCLUDE_DIR}/" DESTINATION include/fast-lib/external)
########

# Add support for merging static libraries
include(MergeStaticLibraries)

# Add subdirectories
add_subdirectory(communication)
add_subdirectory(serialization)
if(BUILD_EXAMPLES)
	enable_testing()
	add_subdirectory(examples)
endif()
