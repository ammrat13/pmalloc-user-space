# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>


cmake_dependent_option(
  PMALLOC_ROUND_PAGESIZE
  "Round the page size provided up to the nearest physical page size"
  ON "PMALLOC_WIN32"
  OFF)


cmake_dependent_option(
  PMALLOC_AGGRESSIVE_PAGESIZE_CHECKS
  "DEBUG: Assert that the normal page size is 4K and the huge page size is 2M"
  OFF "PMALLOC_WIN32; PMALLOC_ROUND_PAGESIZE"
  OFF
)
