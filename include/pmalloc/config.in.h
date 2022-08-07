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


/** \brief Allow the user to allocate in chunks bigger than a page
 *
 * If this option is set and the user requests a chunk of memory larger than the
 * set page size, the allocation will succeed and the new chunk will be placed
 * in its own page, possibly rounded up. If this option is unset, the allocation
 * will fail.
 */
#cmakedefine PMALLOC_MULTIPAGE_ALLOC

/** \brief Page size to use when creating a pool if none is specified
 * \sa pmalloc_create_pool()
 */
#cmakedefine PMALLOC_DEFAULT_PAGESIZE @PMALLOC_DEFAULT_PAGESIZE@

/** \brief Alignment to use for allocation when none is specified
 * \sa pmalloc()
 */
#cmakedefine PMALLOC_DEFAULT_ALIGNMENT @PMALLOC_DEFAULT_ALIGNMENT@


/** \brief Defined if the target platform is Linux (not just UNIX) */
#cmakedefine PMALLOC_LINUX

#if defined(PMALLOC_LINUX)
    /** \brief Round the page size up to the nearest physical page */
#   cmakedefine PMALLOC_ROUND_PAGESIZE
    /** \brief Use huge pages */
#   cmakedefine PMALLOC_HUGETLB
    /** \brief Where procfs is mounted */
#   cmakedefine PMALLOC_PROC_MOUNT "@PMALLOC_PROC_MOUNT@"
    /** \brief What line to read in /proc/meminfo to find the page size */
#   cmakedefine PMALLOC_MEMINFO_HUGEPAGE "@PMALLOC_MEMINFO_HUGEPAGE@"
    /** \brief The largest size of /proc/meminfo we can deal with */
#   cmakedefine PMALLOC_MEMINFO_MAXSIZE @PMALLOC_MEMINFO_MAXSIZE@

    /** \brief Assert that page sizes are exactly their "normal" values */
#   cmakedefine PMALLOC_AGGRESSIVE_PAGESIZE_CHECKS
#endif


/** \brief Defined if the target platform is Windows */
#cmakedefine PMALLOC_WIN32


/** \brief Defined if the functions should be thread-safe */
#cmakedefine PMALLOC_THREADS

#if defined(PMALLOC_THREADS)
    /** \brief Defined if the thread library found was `pthreads` */
#   cmakedefine PMALLOC_PTHREADS
    /** \brief Defined if the thread library found was for Windows */
#   cmakedefine PMALLOC_WIN32_THREADS
#endif  // PMALLOC_THREADS

/**@}*/

#endif  // PMALLOC_CONFIG_H_
