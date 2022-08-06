# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

include(CMakeDependentOption)


set(PMALLOC_HUGETLB_FLAGS "(0)")


cmake_dependent_option(
  PMALLOC_ROUND_PAGESIZE
  "Round the page size provided up to the nearest physical page size"
  ON "PMALLOC_LINUX"
  OFF)

cmake_dependent_option(
  PMALLOC_PAGESIZE_HARDCODED
  "Use a hard-coded page size instead of sysconf(_SC_PAGESIZE) or /proc/meminfo"
  OFF "PMALLOC_LINUX; PMALLOC_ROUND_PAGESIZE"
  OFF)


cmake_dependent_option(
  PMALLOC_HUGETLB
  "Use huge pages instead of normal sized pages"
  OFF "PMALLOC_LINUX; PMALLOC_ROUND_PAGESIZE; NOT PMALLOC_PAGESIZE_HARDCODED"
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

  set(PMALLOC_HUGETLB_FLAGS "(MAP_HUGETLB)")
  return()
endif()


if(PMALLOC_LINUX AND PMALLOC_ROUND_PAGESIZE AND PMALLOC_PAGESIZE_HARDCODED)
  set(
    PMALLOC_PAGESIZE 4096
    CACHE STRING "Size in bytes of the hard-coded page")

  if(NOT PMALLOC_PAGESIZE EQUAL 4096 AND CMAKE_SYSTEM_VERSION VERSION_LESS "3.8")
    message(FATAL_ERROR "Linux ${CMAKE_SYSTEM_VERSION} does not support hard-coded page sizes")
  endif()

  if(PMALLOC_PAGESIZE EQUAL 2097152)
    set(PMALLOC_HUGETLB_FLAGS "(MMAP_HUGE_2MB)")
  elseif(PMALLOC_PAGESIZE EQUAL 1073741824)
    set(PMALLOC_HUGETLB_FLAGS "(MMAP_HUGE_1GB)")
  elseif(NOT PMALLOC_PAGESIZE EQUAL 4096)
    message(FATAL_ERROR "Invalid page size: ${PMALLOC_PAGESIZE}")
  endif()
  return()
endif()
