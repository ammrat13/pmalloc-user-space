// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>
/** \file
 *  \defgroup config CMake Configuration
 *  \ingroup public
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
#cmakedefine PMALLOC_UNIX
/** \brief Defined if the target platform is Windows */
#cmakedefine PMALLOC_WIN32

/** \brief Defined if the functions should be thread-safe */
#cmakedefine PMALLOC_REENTRANT

#ifdef PMALLOC_REENTRANT
    /** \brief Defined if the thread library found was `pthreads` */
#   cmakedefine PMALLOC_REENTRANT_PTHREADS
    /** \brief Defined if the thread library found was for Windows */
#   cmakedefine PMALLOC_REENTRANT_WIN32_THREADS
#endif  // PMALLOC_REENTRANT

#endif  // PMALLOC_CONFIG_H_
