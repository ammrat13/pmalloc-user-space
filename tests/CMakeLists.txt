# SPDX-License-Identifier: GPL-2.0
# Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>


# Function to add a "simple" test. That is, a test which is just a single file
# and just links with `pmalloc`.
function(add_simple_test test_dir test_name)

  # If we're given an argument, the first one is the long name of the test.
  if(ARGC GREATER_EQUAL 3)
    list(GET ARGV 2 test_long_name)
  else()
    set(test_long_name "${test_name}")
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
  if(ARGC GREATER_EQUAL 4)
    list(SUBLIST ARGV 3 -1 test_props)
    set_tests_properties("${test_long_name}" PROPERTIES ${test_props})
  endif()
endfunction()


add_simple_test(
  "simple" "create-destroy"
  "Create and destroy pool"
  LABELS "Simple\\\;Memcheck")
add_simple_test(
  "simple" "create-protect-destroy"
  "Create, protect, then destroy pool"
  LABELS "Simple\\\;Memcheck")

add_simple_test(
  "alloc" "simple"
  "Allocate data from pool"
  LABELS "Allocation\\\;Memcheck")
add_simple_test(
  "alloc" "alignment"
  "Allocate data with alignment"
  LABELS "Allocation\\\;Memcheck")
add_simple_test(
  "alloc" "multipage"
  "Allocate in multiple pages"
  LABELS "Allocation\\\;Memcheck")

add_simple_test(
  "protect" "simple"
  "Protect data inside pool"
  LABELS "Protection\\\;Memcheck")
add_simple_test(
  "protect" "write"
  "Fail to write protected data"
  WILL_FAIL TRUE LABELS "Protection")
add_simple_test(
  "protect/multiple-alloc" "read-ro"
  "Allocate and protect multiple times"
  LABELS "Protection\\\;Memcheck")
add_simple_test(
  "protect/multiple-alloc" "write-ro"
  "Ensure protection work with multiple"
  WILL_FAIL TRUE LABELS "Protection")
