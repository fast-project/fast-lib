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
set(FASTLIB "fastlib")

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

### Define header, source and lib files
# Headers
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
set(HEADERS
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/communicator.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/mqtt_communicator.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/serializable.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/log.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/message/agent/init.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/message/agent/init_agent.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/message/agent/stop_monitor.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/message/migfra/pci_id.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/message/migfra/time_measurement.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/message/migfra/result.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/include/fast-lib/message/migfra/task.hpp"
)

# Source
set(SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/src/mqtt_communicator.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/serializable.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/message/agent/init.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/message/agent/init_agent.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/message/agent/stop_monitor.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/message/migfra/pci_id.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/message/migfra/time_measurement.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/message/migfra/result.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/message/migfra/task.cpp"
)

# Add library
add_library(${FASTLIB} ${SRC} ${HEADERS})

# Enable support for external projects
include(ExternalProject)

########
# Boost
#include(add_boost)
#
#get_property(BOOST_REGEX_LIBRARY TARGET ${BoostRegexLibs} PROPERTY LOCATION)
#get_property(BOOST_TIMER_LIBRARY TARGET ${BoostTimerLibs} PROPERTY LOCATION)
#add_dependencies(${FASTLIB} "${BoostRegexLibs}" "${BoostTimerLibs}")
#set(LIBS "${BOOST_REGEX_LIBRARY}" "${BOOST_TIMER_LIBRARY}")
#if(ENABLE_LOGGING)
#	add_dependencies(${FASTLIB} "${BoostLogLibs}")
#	get_property(BOOST_LOG_LIBRARY TARGET ${BoostLogLibs} PROPERTY LOCATION)
#	get_property(BOOST_CHRONO_LIBRARY TARGET ${BoostChronoLibs} PROPERTY LOCATION) # dependency of boost.log
#	get_property(BOOST_DATE_TIME_LIBRARY TARGET ${BoostDateTimeLibs} PROPERTY LOCATION) # dependency of boost.log
#	get_property(BOOST_FILESYSTEM_LIBRARY TARGET ${BoostFilesystemLibs} PROPERTY LOCATION) # dependency of boost.log
#	get_property(BOOST_THREAD_LIBRARY TARGET ${BoostThreadLibs} PROPERTY LOCATION) # dependency of boost.log
#	get_property(BOOST_SYSTEM_LIBRARY TARGET ${BoostSystemLibs} PROPERTY LOCATION) # dependency of boost.log
#	set(LIBS "${LIBS}" "${BOOST_LOG_LIBRARY}" "${BOOST_CHRONO_LIBRARY}" "${BOOST_DATE_TIME_LIBRARY}" "${BOOST_FILESYSTEM_LIBRARY}" "${BOOST_THREAD_LIBRARY}" "${BOOST_SYSTEM_LIBRARY}")
#endif()
#
#include_directories(SYSTEM ${BoostSourceDir})
#
## Install headers
#install(DIRECTORY "${BoostSourceDir}/boost" DESTINATION include/fast-lib/external)
########

########
# mosquitto
ExternalProject_Add(mosquitto
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/mosquitto-1.3.5
	CMAKE_COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} cmake
	CMAKE_ARGS -DWITH_SRV=OFF -DWITH_TLS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_C_FLAGS=-fpic -DCMAKE_CXX_FLAGS=-fpic)

add_dependencies(${FASTLIB} "mosquitto")

# Store mosquitto variables
ExternalProject_Get_Property(mosquitto install_dir)
set(MOSQUITTO_LIBRARIES ${install_dir}/lib/libmosquittopp.a ${install_dir}/lib/libmosquitto.a)
set(MOSQUITTO_INCLUDE_DIR "${install_dir}/include")
set(LIBS "${LIBS}" "${MOSQUITTO_LIBRARIES}")

include_directories(SYSTEM "${MOSQUITTO_INCLUDE_DIR}")

# Install headers
install(DIRECTORY "${MOSQUITTO_INCLUDE_DIR}/" DESTINATION include/fast-lib/external)
########

########
# yaml-cpp
ExternalProject_Add(yaml-cpp
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/yaml-cpp
	CMAKE_COMMAND CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER} cmake
	CMAKE_ARGS -DCMAKE_CXX_FLAGS=-fpic -DBUILD_SHARED_LIBS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>)

add_dependencies(${FASTLIB} "yaml-cpp")

# Store yaml-cpp variables
ExternalProject_Get_Property(yaml-cpp install_dir)
set(YAMLCPP_LIBRARY "${install_dir}/lib/libyaml-cpp.a")
set(YAMLCPP_INCLUDE_DIR "${install_dir}/include")
set(LIBS "${LIBS}" "${YAMLCPP_LIBRARY}")

include_directories(SYSTEM "${YAMLCPP_INCLUDE_DIR}")

# Install headers
install(DIRECTORY "${YAMLCPP_INCLUDE_DIR}/" DESTINATION include/fast-lib/external)
########

########
# spdlog
#ExternalProject_Add(spdlog
#	GIT_REPOSITORY https://github.com/gabime/spdlog.git
#	GIT_TAG 
#	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
#	CONFIGURE_COMMAND ""
#	BUILD_COMMAND ""
#	INSTALL_COMMAND "cp -r <SOURCE_DIR>/include <INSTALL_DIR>"
#)


#add_dependencies(${FASTLIB} "spdlog")

# Store spdlog variables
#ExternalProject_Get_Property(spdlog install_dir)
#set(SPDLOG_LIBRARY "${install_dir}/lib/libspdlog.a")
#set(SPDLOG_INCLUDE_DIR "${install_dir}/include")
#set(LIBS "${LIBS}" "${SPDLOG_LIBRARY}")

set(SPDLOG_INCLUDE_DIR "vendor/spdlog/include")
include_directories(SYSTEM "${SPDLOG_INCLUDE_DIR}")

# Install headers
install(DIRECTORY "${SPDLOG_INCLUDE_DIR}/" DESTINATION include/fast-lib/external)
########

# Merge with dependent libraries
include(MergeStaticLibraries)
MERGE_STATIC_LIBRARIES(${FASTLIB} ALL "${LIBS}")

# As librt should always be available it is linked dynamically and therefore has to be linked in applications
# that use fast-lib as well.
target_link_libraries(${FASTLIB} -lrt)

# Install
install(TARGETS "${FASTLIB}"
	LIBRARY DESTINATION "lib"
	ARCHIVE DESTINATION "lib")
install(DIRECTORY "include/"
	DESTINATION "include")

# Testing
if(BUILD_EXAMPLES)
	enable_testing()
	add_subdirectory(examples)
endif()
