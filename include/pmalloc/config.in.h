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
