#
# This file is part of fast-lib.
# Copyright (C) 2015 RWTH Aachen University - ACS
#
# This file is licensed under the GNU Lesser General Public License Version 3
# Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
#

### Define source files.
set(HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/communicator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/mqtt_communicator.hpp)
set(SRC ${CMAKE_CURRENT_SOURCE_DIR}/mqtt_communicator.cpp)

### Build and installation targets
# Add library
add_library(${FASTLIB_COMMUNICATION} ${SRC})
target_link_libraries(${FASTLIB_COMMUNICATION} ${LIBS})
install(TARGETS ${FASTLIB_COMMUNICATION}
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)
install(FILES ${HEADERS}
	DESTINATION include/fast-lib/communication)
