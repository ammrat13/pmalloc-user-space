// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>
/** \file
 *  \defgroup private Private Components
 *  \brief Private internals of `pmalloc`
 *
 * @{
 */

#ifndef PMALLOC_INTERNALS_H_
#define PMALLOC_INTERNALS_H_

#include <stdbool.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/arch.h"


typedef struct pmalloc_page_header_t pmalloc_page_header_t;


/** \brief Metadata for each page
 *
 * Each page in the linked list requires some metadata, like the pointer to the
 * next page, the offset of the "base pointer", and whether or not it's read
 * only. This structure contains all that. It's placed at the very start of a
 * page.
 *
 * Allocation within a page goes from high memory to low memory. This is
 * apparently more efficient according to <a
 * href="http://fitzgeraldnick.com/2019/11/01/always-bump-downwards.html">
 * Always Bump Downward by Nick Fitzgerald </a>. As such, the layout of a page
 * is as shown below. The boundary pointer follows the "closed stack"
 * convention, where it points to the first byte of allocated space.
 * <pre>
 * +--------+--------------------+-----------+
 * |  Page  |        Free        | Allocated |
 * | Header |        Space       |   Space   |
 * +--------+--------------------+-----------+
 *                               ^
 *                            Boundary
 *                            Pointer
 * </pre>
 *
 * Pages are allocated with a platform specific function. It's possible we get
 * more data than we need. Thus, we store the actual size of the page here.
 */
struct pmalloc_page_header_t {
    pmalloc_page_header_t *next;  ///< Next page in the linked list
    size_t page_size;  ///< The size of this page in bytes

    /** \brief "Boundary pointer"'s offset from the start of the page
     *
     * The allocator grows down because it's more efficient to do so. Just as
     * with a stack with its stack pointer, we have a "boundary pointer" that
     * starts at the end of the page and grows downward.
     */
    size_t bp_offset;

    bool ro;  ///< Whether this page has (ever) been marked as read only.
};


/** \brief Metadata representing a pool
 *
 * Pools require some metadata to function. For instance, a pool needs to know
 * what pages it contains, what size to use when allocating a new page, and a
 * lock for mutual exclusion. This structure contains that. It's allocated to
 * the "normal" heap and passed back to the user as a handle.
 *
 * Recall that a pool is treated as a linked-list of pages, and this structure
 * points to the head of the list.
 */
struct pmalloc_pool_t {
    pmalloc_page_header_t *head;  ///< First page in the linked list
    size_t page_size;  ///< How much to allocate at once in bytes

#if defined(PMALLOC_THREADS) || defined(DOXYGEN)
    /** \brief Mutual exclusion on the pool
     *
     * Whenever anyone tries to do any action on the pool, they must hold this
     * lock. One would think this would be inefficient, and that page-level
     * locks would be more performant. In reality, since all allocations happen
     * from the front-most page, that lock just replaces this one. It also leads
     * to more complexity.
     */
    pmalloc_mutex_t mutex;
#endif
};


/** \brief Round down `x` to the nearest multiple of `m` */
static inline size_t pmalloc_round_down(size_t x, size_t m) {
    return (x / m) * m;
}

/** \brief Round up `x` to the nearest multiple of `m` */
static inline size_t pmalloc_round_up(size_t x, size_t m) {
    return ((x + m - 1) / m) * m;
}

/**@}*/

#endif  // PMALLOC_INTERNALS_H_
