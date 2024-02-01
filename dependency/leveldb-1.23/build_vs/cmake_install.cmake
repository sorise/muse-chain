# Install script for directory: E:/Codes/clion/grammar/dependency/leveldb-1.23

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/leveldb")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Debug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/leveldb.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/leveldb" TYPE FILE FILES
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/c.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/cache.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/comparator.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/db.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/dumpfile.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/env.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/export.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/filter_policy.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/iterator.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/options.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/slice.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/status.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/table_builder.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/table.h"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/include/leveldb/write_batch.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/leveldb/leveldbTargets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/leveldb/leveldbTargets.cmake"
         "E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/CMakeFiles/Export/f90a79f6c24c38ae6b0a9cccec147da8/leveldbTargets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/leveldb/leveldbTargets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/leveldb/leveldbTargets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/leveldb" TYPE FILE FILES "E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/CMakeFiles/Export/f90a79f6c24c38ae6b0a9cccec147da8/leveldbTargets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/leveldb" TYPE FILE FILES "E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/CMakeFiles/Export/f90a79f6c24c38ae6b0a9cccec147da8/leveldbTargets-debug.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/leveldb" TYPE FILE FILES
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/cmake/leveldbConfig.cmake"
    "E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/cmake/leveldbConfigVersion.cmake"
    )
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/third_party/googletest/cmake_install.cmake")
  include("E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/third_party/benchmark/cmake_install.cmake")

endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "E:/Codes/clion/grammar/dependency/leveldb-1.23/build_vs/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
