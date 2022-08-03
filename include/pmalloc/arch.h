// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>
/** \file
 *  \defgroup arch Architecture Specific Code
 *  \ingroup private
 *  \brief Code that differs on different operating systems
 *
 * To facilitate testing, the core functionality of `pmalloc` is isolated from
 * the platform specific code. This is the platform (architecture) specific
 * code. It contains mechanisms to allocate and free heap memory, allocate and
 * free pages, and lock for mutual exclusion.
 *
 * @{
 */

#ifndef PMALLOC_ARCH_H_
#define PMALLOC_ARCH_H_

#include "pmalloc/config.h"


/** \defgroup heap Heap Functions
 *  \brief Platform specific code to allocate objects on the heap
 *
 * @{
 */

/** \brief Allocate `size` bytes on the heap and return the new object */
void *pmalloc_heap_alloc(size_t size);
/** \brief Free the object on the heap pointed to by `ptr` */
void pmalloc_heap_free(void *ptr);

/**@}*/

/** \defgroup page Paging Functions
 *  \brief Platform specific code to manage pages
 *
 * @{
 */

/** \brief Allocate consecutive pages spanning at least `size` bytes
 * \param [inout] size How many consecutive bytes to reserve. Return the size
 *                     actually allocated.
 * \return Pointer to the start of the memory region allocated
 */
void *pmalloc_page_alloc(size_t *size);
/** \brief Free the pages from `ptr` to `ptr+size-1` */
void pmalloc_page_free(void *ptr, size_t size);
/** \brief Mark the pages from `ptr` to `ptr+size-1` as readonly */
void pmalloc_page_mark_ro(void *ptr, size_t size);

/**@}*/


/** \defgroup lock Mutual Exclusion
 *  \brief Platform specific code for mutual exclusion
 *
 * @{
 */

#if defined(PMALLOC_REENTRANT) || defined(DOXYGEN)
#   if defined(PMALLOC_REENTRANT_PTHREADS) || defined(DOXYGEN)

        #include <pthread.h>

        /** \brief What type to use for mutexes
         *
         * On Linux, this is taken from `pthreads`. Windows hasn't been
         * implemented yet, but there it will likely be a `CriticalSection`.
         */
        typedef pthread_mutex_t pmalloc_mutex_t;

#   elif defined(PMALLOC_REENTRANT_WIN32_THREADS)
#       error "Windows is not currently supported"
#   endif
#endif

/** \brief Acquire a mutex */
void pmalloc_mutex_lock(pmalloc_mutex_t *mutex);
/** \brief Release a mutex */
void pmalloc_mutex_unlock(pmalloc_mutex_t *mutex);

/**@}*/

/**@}*/

#endif  // PMALLOC_ARCH_H_
