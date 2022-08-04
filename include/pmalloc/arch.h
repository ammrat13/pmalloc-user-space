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

#include <stddef.h>

#include "pmalloc/config.h"


/** \defgroup heap Heap Functions
 *  \brief Platform specific code to allocate and free pools on the heap
 *
 * \sa pmalloc_pool_t
 *
 * @{
 */

/** \brief Allocate a pmalloc_pool_t on the "normal" heap */
void *pmalloc_alloc_pool(void);
/** \brief Free a pmalloc_pool_t */
void pmalloc_free_pool(void *ptr);

/**@}*/

/** \defgroup page Paging Functions
 *  \brief Platform specific code to manage pages
 *
 * @{
 */

/** \brief Allocate consecutive pages spanning at least `size` bytes
 * \param [inout] length How many consecutive bytes to reserve. Return the
 *                       length actually allocated.
 * \return Pointer to the start of the memory region allocated
 */
void *pmalloc_alloc_page(size_t *length);
/** \brief Free the pages from `ptr` to `ptr+length-1` */
void pmalloc_free_page(void *ptr, size_t length);
/** \brief Mark the pages from `ptr` to `ptr+length-1` as readonly */
void pmalloc_markro_page(void *ptr, size_t length);

/**@}*/


/** \defgroup lock Mutual Exclusion
 *  \brief Platform specific code for mutual exclusion
 *
 * @{
 */

#if defined(PMALLOC_THREADS) || defined(DOXYGEN)
#   if defined(PMALLOC_PTHREADS) || defined(DOXYGEN)

        #include <pthread.h>

        /** \brief What type to use for mutexes
         *
         * If available, this is taken from `pthreads`. Windows hasn't been
         * implemented yet, but there it will likely be a `CriticalSection`.
         */
        typedef pthread_mutex_t pmalloc_mutex_t;

#   elif defined(PMALLOC_WIN32_THREADS)
#       error "Windows is not currently supported"
#   endif
#endif

/** \brief Initialize a mutex for use */
void pmalloc_mutex_init(pmalloc_mutex_t *mutex);
/** \brief Acquire a mutex */
void pmalloc_mutex_lock(pmalloc_mutex_t *mutex);
/** \brief Release a mutex */
void pmalloc_mutex_unlock(pmalloc_mutex_t *mutex);

/**@}*/

/**@}*/

#endif  // PMALLOC_ARCH_H_