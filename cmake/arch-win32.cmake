# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>


cmake_dependent_option(
  PMALLOC_ROUND_PAGESIZE
  "Round the page size provided up to the nearest physical page size"
  ON "PMALLOC_WIN32"
  OFF)


# On Windows, static and shared libraries use the same file name. Ninja will
# also complain that pmalloc.lib is built twice. Avoid this.
if(BUILD_STATIC_LIBS AND BUILD_SHARED_LIBS)
  message(FATAL_ERROR
    "Static and shared libraries can't be built at the same time on Windows")
endif()

# Set these variables so we know what mode we're building in. We need to know
# this for DLL exports.
set(PMALLOC_IS_STATIC "${BUILD_STATIC_LIBS}")
set(PMALLOC_IS_SHARED "${BUILD_SHARED_LIBS}")

# Workaround for Windows not having RPATH. The test executables and the
# libraries have to be in the same directory for Windows to find them. This is
# fine since the build is fairly simple.
if(BUILD_TESTING)
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()


cmake_dependent_option(
  PMALLOC_AGGRESSIVE_PAGESIZE_CHECKS
  "DEBUG: Assert that the normal page size is 4K and the huge page size is 2M"
  OFF "PMALLOC_WIN32; PMALLOC_ROUND_PAGESIZE"
  OFF
)
