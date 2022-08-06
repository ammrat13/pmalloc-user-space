// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>
/** \file
 *  \defgroup config CMake Configuration
 *  \ingroup private
 *  \brief Header containing variables configured by CMake
 *
 * This project is built using CMake. It provides variables the library needs to
 * configure itself, like what platform it's targeting and what threading
 * libraries are available. This is where those variables are injected.
 *
 * @{
 */

// NOLINT(build/header_guard)
#ifndef PMALLOC_CONFIG_H_
#define PMALLOC_CONFIG_H_

/** \brief Defined if the target platform is Unix-like, including `APPLE` */
#cmakedefine PMALLOC_LINUX

#if defined(PMALLOC_LINUX)

    /** \brief Extra arguments to pass to `mmap` to get the right page size */
#   cmakedefine PMALLOC_MMAP_EXTRA_FLAGS @PMALLOC_MMAP_EXTRA_FLAGS@

    /** \brief Round the page size up to the nearest physical page */
#   cmakedefine PMALLOC_ROUND_PAGESIZE
    /** \brief Use a hard-coded user-supplied page size */
#   cmakedefine PMALLOC_PAGESIZE_HARDCODED

#   if !defined(PMALLOC_PAGESIZE_HARDCODED)
        /** \brief Use huge pages */
#       cmakedefine PMALLOC_HUGETLB
        /** \brief What line to read in /proc/meminfo to find the page size */
#       cmakedefine PMALLOC_HUGETLB_MEMINFO "@PMALLOC_HUGETLB_MEMINFO@"
#   else
        /** \brief The hard-coded size of physical pages */
#       cmakedefine PMALLOC_PAGESIZE @PMALLOC_PAGESIZE@
#   endif
#endif


/** \brief Defined if the target platform is Windows */
#cmakedefine PMALLOC_WIN32


/** \brief Defined if the functions should be thread-safe */
#cmakedefine PMALLOC_THREADS

#ifdef PMALLOC_THREADS
    /** \brief Defined if the thread library found was `pthreads` */
#   cmakedefine PMALLOC_PTHREADS
    /** \brief Defined if the thread library found was for Windows */
#   cmakedefine PMALLOC_WIN32_THREADS
#endif  // PMALLOC_THREADS

/**@}*/

#endif  // PMALLOC_CONFIG_H_
