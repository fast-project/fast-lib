#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.maidsafe.net/licenses                                                            #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
#                                                                                                  #
#==================================================================================================#
#                                                                                                  #
#  Sets up Boost using ExternalProject_Add.                                                        #
#                                                                                                  #
#  Only the first 2 variables should require regular maintenance, i.e. BoostVersion & BoostSHA1.   #
#                                                                                                  #
#  If USE_BOOST_CACHE is set, boost is downloaded, extracted and built to a directory outside of   #
#  the MaidSafe build tree.  The chosen directory can be set in BOOST_CACHE_DIR, or if this is     #
#  empty, an appropriate default is chosen for the given platform.                                 #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    BoostSourceDir (required for subsequent include_directories calls) and per-library            #
#    variables defining the libraries, e.g. BoostDateTimeLibs, BoostFilesystemLibs.                #
#                                                                                                  #
#==================================================================================================#
#                                                                                                  #
# Modified for autopin+                                                                            #
#                                                                                                  #
#==================================================================================================#


set(BoostVersion 1.55.0)
set(BoostSHA1 cef9a0cc7084b1d639e06cd3bc34e4251524c840)

# Helper function to convert underscores to CamelCase
function(ms_underscores_to_camel_case VarIn VarOut)
  string(REPLACE "_" ";" Pieces ${VarIn})
  foreach(Part ${Pieces})
    string(SUBSTRING ${Part} 0 1 Initial)
    string(SUBSTRING ${Part} 1 -1 Part)
    string(TOUPPER ${Initial} Initial)
    set(CamelCase ${CamelCase}${Initial}${Part})
  endforeach()
  set(${VarOut} ${CamelCase} PARENT_SCOPE)
endfunction()


# Create build folder name derived from version
string(REGEX REPLACE "beta\\.([0-9])$" "beta\\1" BoostFolderName ${BoostVersion})
string(REPLACE "." "_" BoostFolderName ${BoostFolderName})
set(BoostFolderName boost_${BoostFolderName})

# If user wants to use a cache copy of Boost, get the path to this location.
if(USE_BOOST_CACHE)
  if(BOOST_CACHE_DIR)
    file(TO_CMAKE_PATH "${BOOST_CACHE_DIR}" BoostCacheDir)
  elseif(WIN32)
    ms_get_temp_dir()
    set(BoostCacheDir "${TempDir}")
  elseif(APPLE)
    set(BoostCacheDir "$ENV{HOME}/Library/Caches")
  else()
    set(BoostCacheDir "$ENV{HOME}/.cache")
  endif()
endif()

# If the cache directory doesn't exist, fall back to use the build root.
if(NOT IS_DIRECTORY "${BoostCacheDir}")
  if(BOOST_CACHE_DIR)
    set(Message "\nThe directory \"${BOOST_CACHE_DIR}\" provided in BOOST_CACHE_DIR doesn't exist.")
    set(Message "${Message}  Falling back to default path at \"${CMAKE_BINARY_DIR}/MaidSafe\"\n")
    message(WARNING "${Message}")
  endif()
  set(BoostCacheDir ${CMAKE_BINARY_DIR})
else()
  if(NOT USE_BOOST_CACHE AND NOT BOOST_CACHE_DIR)
    set(BoostCacheDir "${BoostCacheDir}/MaidSafe")
  endif()
  file(MAKE_DIRECTORY "${BoostCacheDir}")
endif()

# Set up the full path to the source directory
set(BoostSourceDir "${BoostFolderName}_${CMAKE_CXX_COMPILER_ID}_${CMAKE_CXX_COMPILER_VERSION}")
if(HAVE_LIBC++)
  set(BoostSourceDir "${BoostSourceDir}_LibCXX")
endif()
if(HAVE_LIBC++ABI)
  set(BoostSourceDir "${BoostSourceDir}_LibCXXABI")
endif()
string(REPLACE "." "_" BoostSourceDir ${BoostSourceDir})
set(BoostSourceDir "${BoostCacheDir}/${BoostSourceDir}")

# Download boost if required
set(ZipFilePath "${BoostCacheDir}/${BoostFolderName}.tar.bz2")
if(NOT EXISTS ${ZipFilePath})
  message(STATUS "Downloading boost ${BoostVersion} to ${BoostCacheDir}")
endif()
file(DOWNLOAD http://sourceforge.net/projects/boost/files/boost/${BoostVersion}/${BoostFolderName}.tar.bz2/download
     ${ZipFilePath}
     STATUS Status
     SHOW_PROGRESS
     EXPECTED_HASH SHA1=${BoostSHA1}
     )

# Extract boost if required
string(FIND "${Status}" "returning early" Found)
if(Found LESS 0 OR NOT IS_DIRECTORY "${BoostSourceDir}")
  set(BoostExtractFolder "${BoostCacheDir}/boost_unzip")
  file(REMOVE_RECURSE ${BoostExtractFolder})
  file(MAKE_DIRECTORY ${BoostExtractFolder})
  file(COPY ${ZipFilePath} DESTINATION ${BoostExtractFolder})
  message(STATUS "Extracting boost ${BoostVersion} to ${BoostExtractFolder}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${BoostFolderName}.tar.bz2
                  WORKING_DIRECTORY ${BoostExtractFolder}
                  RESULT_VARIABLE Result
                  )
  if(NOT Result EQUAL 0)
    message(FATAL_ERROR "Failed extracting boost ${BoostVersion} to ${BoostExtractFolder}")
  endif()
  file(REMOVE ${BoostExtractFolder}/${BoostFolderName}.tar.bz2)

  # Get the path to the extracted folder
  file(GLOB ExtractedDir "${BoostExtractFolder}/*")
  list(LENGTH ExtractedDir n)
  if(NOT n EQUAL 1 OR NOT IS_DIRECTORY ${ExtractedDir})
    message(FATAL_ERROR "Failed extracting boost ${BoostVersion} to ${BoostExtractFolder}")
  endif()
  file(RENAME ${ExtractedDir} ${BoostSourceDir})
  file(REMOVE_RECURSE ${BoostExtractFolder})
endif()


# Build b2 (bjam) if required
unset(b2Path CACHE)
find_program(b2Path NAMES b2 PATHS ${BoostSourceDir} NO_DEFAULT_PATH)
if(NOT b2Path)
  message(STATUS "Building b2 (bjam)")
  set(b2Bootstrap "./bootstrap.sh")
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    list(APPEND b2Bootstrap --with-toolset=clang)
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    list(APPEND b2Bootstrap --with-toolset=gcc)
  endif()
  execute_process(COMMAND ${b2Bootstrap} WORKING_DIRECTORY ${BoostSourceDir}
                  RESULT_VARIABLE Result OUTPUT_VARIABLE Output ERROR_VARIABLE Error)
  if(NOT Result EQUAL 0)
    message(FATAL_ERROR "Failed running ${b2Bootstrap}:\n${Output}\n${Error}\n")
  endif()
endif()
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BoostSourceDir}/Build)

# Expose BoostSourceDir to parent scope
set(BoostSourceDir ${BoostSourceDir})

# Set up general b2 (bjam) command line arguments
set(b2Args <SOURCE_DIR>/b2
           link=static
           threading=multi
           runtime-link=shared
           --build-dir=Build
           stage
           -d+2
           --hash
           )
if("${CMAKE_BUILD_TYPE}" STREQUAL "ReleaseNoInline")
  list(APPEND b2Args cxxflags="${RELEASENOINLINE_FLAGS}")
endif()
if("${CMAKE_BUILD_TYPE}" STREQUAL "DebugLibStdcxx")
  list(APPEND b2Args define=_GLIBCXX_DEBUG)
endif()

# Set up platform-specific b2 (bjam) command line arguments
  list(APPEND b2Args variant=release cxxflags=-fPIC cxxflags=-std=c++11 -sNO_BZIP2=1 --layout=tagged)
  # Need to configure the toolset based on whatever version CMAKE_CXX_COMPILER is
  string(REGEX MATCH "[0-9]+\\.[0-9]+" ToolsetVer "${CMAKE_CXX_COMPILER_VERSION}")
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    list(APPEND b2Args toolset=clang-${ToolsetVer})
    if(HAVE_LIBC++)
      list(APPEND b2Args cxxflags=-stdlib=libc++ linkflags=-stdlib=libc++)
    endif()
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    list(APPEND b2Args toolset=gcc-${ToolsetVer})
  endif()

# Get list of components
execute_process(COMMAND ./b2 --show-libraries WORKING_DIRECTORY ${BoostSourceDir}
                ERROR_QUIET OUTPUT_VARIABLE Output)
string(REGEX REPLACE "(^[^:]+:|[- ])" "" BoostComponents "${Output}")
string(REGEX REPLACE "\n" ";" BoostComponents "${BoostComponents}")

# Build each required component
include(ExternalProject)
foreach(Component ${BoostComponents})
  ExternalProject_Add(
      boost_${Component}
      PREFIX ${CMAKE_BINARY_DIR}/${BoostFolderName}
      SOURCE_DIR ${BoostSourceDir}
      BINARY_DIR ${BoostSourceDir}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND "${b2Args}" --with-${Component}
      INSTALL_COMMAND ""
      LOG_BUILD ON
      )
  ms_underscores_to_camel_case(${Component} CamelCaseComponent)
  add_library(Boost${CamelCaseComponent} STATIC IMPORTED GLOBAL)
    set_target_properties(Boost${CamelCaseComponent} PROPERTIES
                          IMPORTED_LOCATION ${BoostSourceDir}/stage/lib/libboost_${Component}-mt.a
                          LINKER_LANGUAGE CXX)
  set_target_properties(boost_${Component} Boost${CamelCaseComponent} PROPERTIES
                        LABELS Boost FOLDER "Third Party/Boost" EXCLUDE_FROM_ALL TRUE)
  add_dependencies(Boost${CamelCaseComponent} boost_${Component})
  set(Boost${CamelCaseComponent}Libs Boost${CamelCaseComponent})
#  if("${Component}" STREQUAL "locale")
#    if(APPLE)
#      find_library(IconvLib iconv)
#      if(NOT IconvLib)
#        message(FATAL_ERROR "libiconv.dylib must be installed to a standard location.")
#      endif()
#      set(Boost${CamelCaseComponent}Libs Boost${CamelCaseComponent} ${IconvLib})
#    elseif(UNIX)
#      if(BSD)
#        find_library(IconvLib libiconv.a)
#        if(NOT IconvLib)
#          set(Msg "libiconv.a must be installed to a standard location.")
#          set(Msg "  For ${Msg} on FreeBSD 10 or later, run\n  pkg install libiconv")
#          message(FATAL_ERROR "${Msg}")
#        endif()
#        set(Boost${CamelCaseComponent}Libs Boost${CamelCaseComponent} ${IconvLib})
#      else()
#        find_library(Icui18nLib libicui18n.a)
#        find_library(IcuucLib libicuuc.a)
#        find_library(IcudataLib libicudata.a)
#        if(NOT Icui18nLib OR NOT IcuucLib OR NOT IcudataLib)
#          set(Msg "libicui18n.a, libicuuc.a & licudata.a must be installed to a standard location.")
#          set(Msg "  For ${Msg} on Ubuntu/Debian, run\n  sudo apt-get install libicu-dev")
#          message(FATAL_ERROR "${Msg}")
#        endif()
#        set(Boost${CamelCaseComponent}Libs Boost${CamelCaseComponent} ${Icui18nLib} ${IcuucLib} ${IcudataLib})
#      endif()
#    else()
#      set(Boost${CamelCaseComponent}Libs Boost${CamelCaseComponent})
#    endif()
#  endif()
  set(Boost${CamelCaseComponent}Libs ${Boost${CamelCaseComponent}Libs})
  list(APPEND AllBoostLibs Boost${CamelCaseComponent})
endforeach()
set(AllBoostLibs ${AllBoostLibs})
add_dependencies(boost_chrono boost_system)
add_dependencies(boost_coroutine boost_context boost_system)
add_dependencies(boost_filesystem boost_system)
add_dependencies(boost_graph boost_regex)
add_dependencies(boost_locale boost_system)
add_dependencies(boost_log boost_chrono boost_date_time boost_filesystem boost_thread)
add_dependencies(boost_thread boost_chrono)
add_dependencies(boost_timer boost_chrono)
add_dependencies(boost_wave boost_chrono boost_date_time boost_filesystem boost_thread)


#==================================================================================================#
# Package                                                                                          #
#==================================================================================================#
#install(DIRECTORY ${BoostSourceDir}/stage/lib DESTINATION .)
#install(DIRECTORY ${BoostSourceDir}/boost DESTINATION include/maidsafe/third_party_libs)
