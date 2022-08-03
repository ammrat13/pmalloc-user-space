// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

// NOLINT(build/header_guard)
#ifndef PMALLOC_CONFIG_H_
#define PMALLOC_CONFIG_H_

#cmakedefine PMALLOC_UNIX
#cmakedefine PMALLOC_WIN32

#cmakedefine PMALLOC_REENTRANT

#ifdef PMALLOC_REENTRANT
#   cmakedefine PMALLOC_REENTRANT_PTHREADS
#   cmakedefine PMALLOC_REENTRANT_WIN32_THREADS
#endif  // PMALLOC_REENTRANT

#endif  // PMALLOC_CONFIG_H_
