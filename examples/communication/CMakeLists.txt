#
# This file is part of fast-lib.
# Copyright (C) 2015 RWTH Aachen University - ACS
#
# This file is licensed under the GNU Lesser General Public License Version 3
# Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
#

### Define source files.
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
include_directories(${PROJECT_SOURCE_DIR})

### Build and installation targets
# Add executable
add_executable(fastlib_example_communication ${SRC})
target_link_libraries(fastlib_example_communication ${FASTLIB_COMMUNICATION})
add_test(communication_tests fastlib_example_communication)
