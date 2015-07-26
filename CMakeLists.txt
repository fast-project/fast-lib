#
# This file is part of fast-lib.
# Copyright (C) 2015 RWTH Aachen University - ACS
#
# This file is licensed under the GNU Lesser General Public License Version 3
# Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
#

cmake_minimum_required(VERSION 2.8)
project(fast-lib)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

# Cache
set(BUILD_SHARED_LIBS ON CACHE BOOL "If ON build shared, else static libraries.")

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
# C-ares
ExternalProject_Add(libcares
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/c-ares-1.10.0
    CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendor/c-ares-1.10.0/configure --prefix=<INSTALL_DIR> CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
    BUILD_IN_SOURCE 0
    BUILD_COMMAND ${MAKE})

# Store c-ares install directory in variable
ExternalProject_Get_Property(libcares install_dir)
set (libcares_path ${install_dir})
include_directories(SYSTEM ${libcares_path}/include)
########

########
# Mosquitto
ExternalProject_Add(libmosquitto
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/mosquitto-1.3.5
    CONFIGURE_COMMAND  CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} cmake -DWITH_TLS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_C_FLAGS=-fpic -DCMAKE_CXX_FLAGS=-fpic -DARES_PATH=${libcares_path} ${CMAKE_CURRENT_SOURCE_DIR}/vendor/mosquitto-1.3.5
    BUILD_IN_SOURCE 0
    BUILD_COMMAND ${MAKE})

# Make sure mosquitto depends on libcares
add_dependencies (libmosquitto libcares)

# Store libmosquitto variable in directory
ExternalProject_Get_Property(libmosquitto install_dir)
set (libmosquitto_path ${install_dir})
include_directories(SYSTEM ${libmosquitto_path}/include)
########

########
# Boost
include(vendor/boost/add_boost.cmake)
include_directories(SYSTEM ${BoostSourceDir})
########

########
# Yaml-Cpp
ExternalProject_Add(libyaml
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/vendor/yaml-cpp-0.5.1
    CONFIGURE_COMMAND  CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} cmake -DCMAKE_CXX_FLAGS=-fpic -DBOOST_ROOT=${BoostSourceDir} -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> ${CMAKE_CURRENT_SOURCE_DIR}/vendor/yaml-cpp-0.5.1
    BUILD_IN_SOURCE 0
    BUILD_COMMAND ${MAKE})

# Store libmosquitto variable in directory
ExternalProject_Get_Property(libyaml install_dir)
set (libyaml_path ${install_dir})
include_directories(SYSTEM ${libyaml_path}/include)
########

set (LIBS ${libmosquitto_path}/lib/libmosquittopp.a ${libmosquitto_path}/lib/libmosquitto.a ${libcares_path}/lib/libcares.a ${libyaml_path}/lib/libyaml-cpp.a -lrt -lboost_regex)

# Add subdirectories
add_subdirectory(communication)
add_subdirectory(serialization)
enable_testing()
add_subdirectory(examples)
