# This is a modificated version of 
# https://raw.githubusercontent.com/romaniucradu/Calculated/master/cmake_modules/FindMosquittopp.cmake
#
# - Find libmosquittopp
# Find the native libmosquittopp includes and libraries
#
#  MOSQUITTOPP_INCLUDE_DIR - where to find mosquittopp.h, etc.
#  MOSQUITTOPP_LIBRARIES   - List of libraries when using libmosquittopp.
#  MOSQUITTOPP_FOUND       - True if libmosquittopp found.

if(MOSQUITTOPP_INCLUDE_DIR)
    # Already in cache, be silent
    set(MOSQUITTOPP_FIND_QUIETLY TRUE)
endif(MOSQUITTOPP_INCLUDE_DIR)

find_path(MOSQUITTOPP_INCLUDE_DIR mosquittopp.h)

find_library(MOSQUITTOPP_LIBRARY NAMES libmosquittopp mosquittopp)

# Handle the QUIETLY and REQUIRED arguments and set MOSQUITTOPP_FOUND to TRUE if
# all listed variables are TRUE.
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(MOSQUITTOPP DEFAULT_MSG MOSQUITTOPP_LIBRARY MOSQUITTOPP_INCLUDE_DIR)

if(MOSQUITTOPP_FOUND)
  set(MOSQUITTOPP_LIBRARIES ${MOSQUITTOPP_LIBRARY})
else(MOSQUITTOPP_FOUND)
  set(MOSQUITTOPP_LIBRARIES)
endif(MOSQUITTOPP_FOUND)

mark_as_advanced(MOSQUITTOPP_INCLUDE_DIR MOSQUITTOPP_LIBRARY)
