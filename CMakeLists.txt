# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>
# See: https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.23)
project(pmalloc LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)

# The directory for CMake configurations isn't set by default
set(INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/pmalloc/cmake/")


# Add targets for both the static and shared libraries. Both of them share the
# same include directories and library base name
foreach(cfg static shared)
  string(TOUPPER ${cfg} lib_type)
  set(target pmalloc-${cfg})

  # CMake doesn't distinguish between PUBLIC and PRIVATE headers. So, we mark
  # all the headers as PRIVATE and install later. We do this by a custom file
  # set.
  add_library(${target} ${lib_type})
  set_target_properties(${target} PROPERTIES
    OUTPUT_NAME ${CMAKE_PROJECT_NAME})
  target_include_directories(${target}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include/")
  target_sources(${target}
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/src/pmalloc.c")
  target_sources(${target}
    PUBLIC FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    FILES "${CMAKE_CURRENT_SOURCE_DIR}/include/pmalloc.h")

  # Install the target under its own export. The export it into a file for
  # later use, and install the export file.
  install(TARGETS ${target}
    EXPORT ${target}-export
    LIBRARY
    FILE_SET HEADERS)
  install(EXPORT ${target}-export
    FILE "${target}.cmake"
    DESTINATION "${INSTALL_CONFIGDIR}"
    NAMESPACE pmalloc::)
endforeach()


# Create and install the package configuration file. Be sure to pass the path
# where all the configurations were installed.
configure_package_config_file("pmalloc-config.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/pmalloc-config.cmake"
  INSTALL_DESTINATION "${INSTALL_CONFIGDIR}"
  PATH_VARS INSTALL_CONFIGDIR)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/pmalloc-config.cmake"
  DESTINATION "${INSTALL_CONFIGDIR}")
