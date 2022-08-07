# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>
# See: https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html

cmake_minimum_required(VERSION 3.23)
project(pmalloc LANGUAGES C)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
include(CTest)

option(BUILD_STATIC_LIBS "Build static libraries" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

option(PMALLOC_MULTIPAGE_ALLOC "Allow allocations larger than one page" ON)
set(PMALLOC_DEFAULT_PAGESIZE 4096 CACHE STRING "Default size of pool pages")
set(PMALLOC_DEFAULT_ALIGNMENT 3 CACHE STRING "Default alignment of objects")

set(
  PMALLOC_INSTALL_CONFIGDIR "${CMAKE_INSTALL_LIBDIR}/pmalloc/cmake/"
  CACHE PATH "Where to install the CMake configuration of this package")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS ON)


# Figure out what platform we're on and set variables accordingly. Die if we
# don't recognize the platform. If we do, include platform specific
# configuration.
if(UNIX)
  if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PMALLOC_LINUX ON)
    set(arch "linux")
  endif()
elseif(WIN32)
  set(PMALLOC_WIN32 ON)
  set(arch "win32")
endif()
if(NOT DEFINED arch)
  message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM}")
endif()
include("cmake/arch-${arch}.cmake" OPTIONAL)

# Whether to have thread-safe functions
set(PMALLOC_THREADS ON CACHE BOOL "Make the functions thread-safe")

# If we want thread safety, we need a thread library. Set variables based on
# which one we find
if(PMALLOC_THREADS)
  find_package(Threads REQUIRED)
  set(PMALLOC_PTHREADS "${CMAKE_USE_PTHREADS_INIT}")
  set(PMALLOC_WIN32_THREADS "${CMAKE_USE_WIN32_THREADS_INIT}")
endif()

# Add configuration information to headers
configure_file(
  "${CMAKE_SOURCE_DIR}/include/pmalloc/config.in.h"
  "${CMAKE_SOURCE_DIR}/include/pmalloc/config.h")

# TODO: Add Windows support
if(arch STREQUAL "win32")
  message(FATAL_ERROR "Windows is not currently supported")
endif()


# Add targets for the libraries we were told to build. Both of them share the
# same include directories and library base name.
foreach(cfg static shared)
  string(TOUPPER ${cfg} lib_type)
  set(target pmalloc-${cfg})

  # Break early if we're not supposed to build this
  if(NOT BUILD_${lib_type}_LIBS)
    continue()
  endif()

  # CMake doesn't distinguish between PUBLIC and PRIVATE headers. So, we mark
  # all the headers as PRIVATE and install later. We do this by a custom file
  # set.
  add_library(${target} ${lib_type})
  set_target_properties(${target} PROPERTIES OUTPUT_NAME ${CMAKE_PROJECT_NAME})
  target_include_directories(${target}
    PRIVATE "${CMAKE_SOURCE_DIR}/include/")
  target_sources(${target}
    PRIVATE
      "${CMAKE_SOURCE_DIR}/src/pmalloc.c"
      "${CMAKE_SOURCE_DIR}/src/arch/${arch}.c")
  if(PMALLOC_THREADS)
    target_link_libraries(${target} PUBLIC Threads::Threads)
  endif()

  # The custom file set of all the headers to install.
  target_sources(${target}
    PUBLIC FILE_SET HEADERS
    BASE_DIRS "${CMAKE_SOURCE_DIR}/include/"
    FILES
      "${CMAKE_SOURCE_DIR}/include/pmalloc/pmalloc.h"
      "${CMAKE_SOURCE_DIR}/include/pmalloc/config.h")

  # Turn warnings into errors. We shouldn't have any warnings to begin with.
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /WX)
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
  endif()

  # Install the target under its own export. The export it into a file for
  # later use, and install the export file.
  install(TARGETS ${target}
    EXPORT ${target}-export
    LIBRARY
    FILE_SET HEADERS)
  install(EXPORT ${target}-export
    FILE "${target}.cmake"
    DESTINATION "${PMALLOC_INSTALL_CONFIGDIR}"
    NAMESPACE pmalloc::)
endforeach()

# Alias the targets for ease of use. Prefer the static library when using CMake.
# Also check that something was built.
if(BUILD_STATIC_LIBS)
  add_library(pmalloc ALIAS pmalloc-static)
elseif(BUILD_SHARED_LIBS)
  add_library(pmalloc ALIAS pmalloc-shared)
else()
  message(FATAL_ERROR "Neither static nor shared libraries were built")
endif()


# Create and install the package configuration file. Be sure to pass the path
# where all the configurations were installed.
configure_package_config_file(
  "${CMAKE_SOURCE_DIR}/cmake/pmalloc-config.cmake.in"
  "${CMAKE_SOURCE_DIR}/cmake/pmalloc-config.cmake"
  INSTALL_DESTINATION "${PMALLOC_INSTALL_CONFIGDIR}")
install(FILES "${CMAKE_SOURCE_DIR}/cmake/pmalloc-config.cmake"
  DESTINATION "${PMALLOC_INSTALL_CONFIGDIR}")


if(BUILD_TESTING)
  add_subdirectory(tests/)
endif()
