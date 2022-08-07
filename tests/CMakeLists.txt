# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

option(PMALLOC_TESTS_VALGRIND "Use valgrind when running some tests" ON)


# Function to add a "simple" test. That is, a test which is just a single file
# and just links with `pmalloc`.
function(add_simple_test test_dir test_name)

  # If we're given an argument, the first one is the long name of the test.
  if(ARGC GREATER_EQUAL 3)
    list(GET ARGV 2 test_long_name)
  else()
    set(test_long_name "${test_name}")
  endif()

  # Get any properties we're given
  if(ARGC GREATER_EQUAL 4)
    list(SUBLIST ARGV 3 -1 test_props)
  endif()

  # Check for valgrind
  set(valgrind OFF)
  list(FIND test_props "VALGRIND" valgrind_sig_idx)
  if(NOT valgrind_sig_idx EQUAL -1)
    math(EXPR valgrind_opt_idx "${valgrind_sig_idx} + 1" OUTPUT_FORMAT DECIMAL)
    list(GET test_props ${valgrind_opt_idx} valgrind_opt)
    if(valgrind_opt STREQUAL "TRUE")
      set(valgrind ON)
    endif()
    list(REMOVE_AT test_props ${valgrind_sig_idx} ${valgrind_opt_idx})
  endif()

  # Substitute hyphens into the test directory for uniqueness. Use this to
  # compute the test target
  string(REPLACE "/" "-" test_dir_hyphen "${test_dir}")
  set(test_target "test-${test_dir_hyphen}_${test_name}")

  # Actually make the test.
  add_executable("${test_target}" "${test_dir}/${test_name}.c")
  target_link_libraries("${test_target}" pmalloc)
  if(PMALLOC_TESTS_VALGRIND AND valgrind)
    add_test(NAME "${test_long_name}" COMMAND
      "valgrind" "--leak-check=full" "$<TARGET_FILE:${test_target}>")
  else()
    add_test(NAME "${test_long_name}" COMMAND "${test_target}")
  endif()

  # Set any given properties
  list(LENGTH test_props test_props_len)
  if(test_props_len GREATER_EQUAL 1)
    set_property(TEST "${test_long_name}" PROPERTY ${test_props})
  endif()
endfunction()


add_simple_test(
  "simple" "create-and-destroy-pool"
  "Create and destroy pool"
  VALGRIND TRUE)

add_simple_test(
  "alloc" "simple"
  "Allocate data from pool"
  VALGRIND TRUE)
add_simple_test(
  "alloc" "alignment"
  "Allocate data with alignment"
  VALGRIND TRUE)
  add_simple_test(
    "alloc" "multipage"
    "Allocate in multiple pages"
    VALGRIND TRUE)
