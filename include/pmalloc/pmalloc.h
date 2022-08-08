// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>
/** \file
 *  \defgroup public Public Interface
 *  \brief Public interface for `pmalloc`
 *
 * The interface downstream consumers use to access this library. Its core
 * functionality is quite small. It's a pool allocator, and thus has functions
 * to create and destroy pools, as well as create an object in a pool. It can
 * also mark a pool as read-only, causing all the objects currently allocated in
 * it to fault on modification. New objects can still be created in that pool,
 * and those can be written to.
 *
 * @{
 */

#ifndef PMALLOC_PMALLOC_H_
#define PMALLOC_PMALLOC_H_

#include <stddef.h>

#include "pmalloc/config.h"

#if (defined(PMALLOC_WIN32) && defined(PMALLOC_IS_SHARED)) || defined(DOXYGEN)
#   if defined(PMALLOC_EXPORTS) || defined(DOXYGEN)
        /** \brief Marker for DLL functions on Windows */
#       define PMALLOC_API __declspec(dllexport)
#   else
#       define PMALLOC_API __declspec(dllimport)
#   endif
#else
#   define PMALLOC_API
#endif


/** \brief Opaque handle to a pool managed by `pmalloc`
 *
 * This library is a pool allocator. The consumer creates pools and allocates
 * objects from them. The pools have metadata associated with them, like where
 * the objects in this pool are in memory, alignment constraints for new
 * objects, and locks for reentrancy. This structure represents that metadata.
 * It's needed by the consumer for performing actions inside a pool.
 *
 * The implementation details of this structure can be found in
 * pmalloc/internals.h.
 *
 * \sa pmalloc_create_pool()
 */
typedef struct pmalloc_pool_t pmalloc_pool_t;


/** \defgroup pool Pool Management
 *  \brief Functions for the creation, destruction, and protection of pools
 *
 * Remember that `pmalloc` is a pool allocator. Objects must be allocated from
 * existing pools. As such, perhaps the most important function in this module
 * is pmalloc_create_custom_pool().
 *
 * @{
 */

/** \brief Create a pool with the specified page size and alignment
 *
 * In this allocator, a pool is a set of "pages", which are just contiguous
 * areas of memory. Each page has the same permissions --- either read and write
 * or read only. In other words, write permissions are issued on a page
 * granularity. As far as `pmalloc` is concerned, these pages can span multiple
 * OS pages or be less than one OS page. They'll all have the same permissions.
 *
 * Objects are created in pages using a simple bump allocator. Importantly, when
 * a page runs out of space, a new page is allocated for the object and the
 * remaining space in the page remains unused. Currently, no attempt is made to
 * reclaim this space. Thus, `page_size` should be chosen to minimize this
 * fragmentation.
 *
 * Additionally, when a pool is protected, all the pages in the pool are marked
 * as read only, and no new writable objects can be allocated there. The space
 * in those old pages is wasted. Again, `page_size` should be chosen to minimize
 * this effect.
 *
 * \param page_size Write permission granularity in bytes. Must be at least `1`
 * \return Opaque handle of the pool created, or `NULL` if `page_size` was `0`
 *
 * \sa pmalloc_destroy_pool()
 */
PMALLOC_API pmalloc_pool_t *pmalloc_create_custom_pool(size_t page_size);

/** \brief Calls pmalloc_create_custom_pool() with the default page size
 * \sa PMALLOC_DEFAULT_PAGE_SIZE
 * \sa pmalloc_create_custom_pool()
 */
static inline pmalloc_pool_t *pmalloc_create_pool(void) {
    return pmalloc_create_custom_pool(PMALLOC_DEFAULT_PAGESIZE);
}


/** \brief Destroy a pool given its handle
 *
 * A pool will leak resources if it isn't destroyed. Thus, this function takes
 * in a pool's handle and destroys all the resources it's holding. It reclaims
 * all its pages and frees its metadata. After this function, the handle passed
 * in is no longer valid, nor are the objects that were present in the pool.
 * Using either of them is undefined behavior.
 *
 * \param [in] pool Handle of the pool to destroy
 */
PMALLOC_API void pmalloc_destroy_pool(pmalloc_pool_t *pool);

/** \brief Mark a pool as read only given its handle
 *
 * A pool can be marked as read only. When that happens, writes are disabled to
 * all the pages in the pool, causing future writes to the pool's objects to
 * fault. More objects can be allocated in the pool, but they will be placed in
 * new pages, with extra space in the read only pages going to waste. For this
 * reason, during pool creation `page_size` should be chosen to minimize this
 * fragmentation.
 *
 * \param [in] pool Handle of the pool to destroy
 */
PMALLOC_API void pmalloc_protect_pool(pmalloc_pool_t *pool);

/**@}*/


/** \defgroup alloc Pool Allocation
 *  \brief Functions to allocate memory in a pool
 *
 * @{
 */

/** \brief Allocate memory in a pool with the specified alignment
 *
 * As an implementation detail, the allocator starts at the top of the page and
 * allocates memory going downward. This is apparently more efficient according
 * to <a href="http://fitzgeraldnick.com/2019/11/01/always-bump-downwards.html">
 * Always Bump Downward by Nick Fitzgerald </a>.
 *
 * The `align` parameter is the log-base-2 of the required alignment. For
 * instance, if an object had to be 8-byte aligned, `align` would be `3`.
 *
 * \param [in] pool Handle of the pool to allocate memory in
 * \param size Number of bytes to allocate
 * \param align The log-base-2 of the alignment needed
 * \return Pointer to the allocated memory
 */
PMALLOC_API void *pmalloc_align(
    pmalloc_pool_t *pool,
    size_t size,
    size_t align);

/** \brief Calls pmalloc_align() with the supplied arguments and the default
 *         alignment
 * \sa pmalloc_align()
 */
static inline void *pmalloc(pmalloc_pool_t *pool, size_t size) {
    return pmalloc_align(pool, size, PMALLOC_DEFAULT_ALIGNMENT);
}

/**@}*/

/**@}*/

#endif  // PMALLOC_PMALLOC_H_
