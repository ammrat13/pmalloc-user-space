// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>

#include "pmalloc/internals.h"


pmalloc_pool_t *pmalloc_create_custom_pool(size_t page_size) {
    // Error checking the arguments. The page size cannot be zero - it just
    // doesn't make sense.
    assert(page_size != 0);
    if (page_size == 0) {
        return NULL;
    }
    // Allocate and return
    pmalloc_pool_t *const ret = pmalloc_alloc_pool();
    ret->head = NULL;
    ret->page_size = page_size;
    #if defined(PMALLOC_THREADS)
        pmalloc_alloc_mutex(&ret->mutex);
    #endif
    return ret;
}

void pmalloc_destroy_pool(pmalloc_pool_t *pool) {
    // Error checking the arguments. Behave like `free` and don't do anything if
    // passed a `NULL` pool.
    assert(pool);
    if (pool == NULL) {
        return;
    }
    // We don't have to lock here. It's undefined behavior to have a race with
    // this and any other function call.

    // Traverse the linked list, freeing all the pages. Make sure we don't read
    // from the pointer once it's destroyed.
    pmalloc_page_header_t *cur = pool->head;
    while (cur != NULL) {
        pmalloc_page_header_t *next = cur->next;
        pmalloc_free_page(cur, cur->page_size);
        cur = next;
    }

    // Destroy the pool and the lock inside it
    #if defined(PMALLOC_THREADS)
        pmalloc_free_mutex(&pool->mutex);
    #endif
    pmalloc_free_pool(pool);
}

void pmalloc_protect_pool(pmalloc_pool_t *pool) {
    // Error checking the arguments. Don't do anything if the argument is
    // `NULL`.
    assert(pool);
    if (pool == NULL) {
        return;
    }
    // Lock
    #if defined(PMALLOC_THREADS)
        pmalloc_lock_mutex(&pool->mutex);
    #endif

    // Traverse the linked list, marking all the pages. Make sure we don't write
    // to a page once it's marked.
    pmalloc_page_header_t *cur = pool->head;
    while (cur != NULL) {
        cur->ro = true;
        pmalloc_markro_page(cur, cur->page_size);
        cur = cur->next;
    }

    // Unlock
    #if defined(PMALLOC_THREADS)
        pmalloc_unlock_mutex(&pool->mutex);
    #endif
}

void *pmalloc_align(pmalloc_pool_t *pool, size_t size, size_t align) {
    // Error checking the arguments. The `pool` obviously has to be non-null.
    // It's not an error to pass zero in for the size, so just return `NULL`.
    assert(pool);
    if (pool == NULL) {
        return NULL;
    }
    if (size == 0) {
        return NULL;
    }
    // Lock. We can optimize by not locking during large allocations, but this
    // leads to simpler logic.
    #if defined(PMALLOC_THREADS)
        pmalloc_lock_mutex(&pool->mutex);
    #endif

    // Compute how much space is needed for allocation. Check to see if we need
    // to allocate a new page.
    bool need_new_page = false;
    const size_t min_page_size =
        pmalloc_round_up(sizeof(pmalloc_page_header_t), 1 << align) +
        size;
    if (pool->page_size < min_page_size) {
        #if defined(PMALLOC_MULTIPAGE_ALLOC)
            need_new_page = true;
        #else
            return NULL;
        #endif
    } else if (pool->head == NULL || pool->head->bp_offset < min_page_size) {
        need_new_page = true;
    }

    // Actually do the allocation
    void *ret;
    if (need_new_page) {
        // Find out what size to use for the new page. Always allocate at least
        // the given page size, and at least enough to hold what we need.
        size_t new_page_size =
            #if defined(PMALLOC_MULTIPAGE_ALLOC)
                (pool->page_size > min_page_size)
                    ? pool->page_size
                    : min_page_size;
            #else
                pool->page_size;
            #endif
        // Allocate the new page
        pmalloc_page_header_t *const new_page =
            pmalloc_alloc_page(&new_page_size);
        assert(new_page_size >= pool->page_size);
        assert(new_page_size >= min_page_size);
        // Set up the fields
        const size_t new_page_bp =
            pmalloc_round_down(new_page_size - size, 1 << align);
        assert(new_page_bp >= sizeof(pmalloc_page_header_t));
        assert(new_page_bp % (1 << align) == 0);
        new_page->page_size = new_page_size;
        new_page->bp_offset = new_page_bp;
        new_page->ro = false;
        // Link it in
        new_page->next = pool->head;
        pool->head = new_page;
        // Return
        ret = (char *) new_page + new_page_bp;
    } else {
        pool->head->bp_offset =
            pmalloc_round_down(pool->head->bp_offset - size, 1 << align);
        assert(pool->head->bp_offset >= sizeof(pmalloc_page_header_t));
        assert(pool->head->bp_offset % (1 << align) == 0);
        ret = (char *) pool->head + pool->head->bp_offset;
    }

    // Unlock
    #if defined(PMALLOC_THREADS)
        pmalloc_unlock_mutex(&pool->mutex);
    #endif
    assert(ret);
    return ret;
}
