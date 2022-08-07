# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

include(CMakeDependentOption)


cmake_dependent_option(
  PMALLOC_ROUND_PAGESIZE
  "Round the page size provided up to the nearest physical page size"
  ON "PMALLOC_LINUX"
  OFF)

cmake_dependent_option(
  PMALLOC_HUGETLB
  "Try to use huge pages if possible, falling back to normal pages otherwise"
  OFF "PMALLOC_LINUX; PMALLOC_ROUND_PAGESIZE"
  OFF
)


if(PMALLOC_HUGETLB)
  if(CMAKE_SYSTEM_VERSION VERSION_LESS "2.6.32")
    message(FATAL_ERROR "Linux ${CMAKE_SYSTEM_VERSION} does not support huge pages")
  endif()

  set(
    PMALLOC_MEMINFO_HUGEPAGE "Hugepagesize:"
    CACHE STRING "What line to read in /proc/meminfo to find the huge page size")
  set(
    PMALLOC_MEMINFO_MAXSIZE 4000
    CACHE STRING "The maximum size of /proc/meminfo")
  return()
endif()


cmake_dependent_option(
  PMALLOC_AGGRESSIVE_PAGESIZE_CHECKS
  "DEBUG: Assert that the normal page size is 4K and the huge page size is 2M"
  OFF "PMALLOC_LINUX; PMALLOC_ROUND_PAGESIZE"
  OFF
)
