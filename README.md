<!---
This file is part of fast-lib.
Copyright (C) 2015 RWTH Aachen University - ACS

This file is licensed under the GNU Lesser General Public License Version 3
Version 3, 29 June 2007. For details see 'LICENSE.md' in the root directory.
-->

# fast-lib
A C++ library for FaST related functionality

Requirements: mosquitto, mosquittopp, yaml-cpp

Build instructions for shared libraries:
mkdir build && cd build
cmake ..
make

Build instructions for static libraries:
mkdir build && cd build
cmake -DBUILD_SHARED_LIBS=OFF ..
make

Testing:
mosquitto -d 2> /dev/null
make test
