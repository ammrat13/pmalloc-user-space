// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>

#include "pmalloc/internals.h"


pmalloc_pool_t *pmalloc_create_custom_pool(size_t page_size) {
    // Error checking the arguments. The page size cannot be zero - it just
    // doesn't make sense.
    assert(page_size != 0);
    if(page_size == 0) {
        return NULL;
    }
    // Allocate and return
    pmalloc_pool_t *ret = pmalloc_alloc_pool();
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
    if(pool == NULL) {
        return;
    }
    // We don't have to lock here. It's undefined behavior to have a race with
    // this and any other function call.

    // Traverse the linked list, freeing all the pages. Make sure we don't read
    // from the pointer once it's destroyed.
    pmalloc_page_header_t *cur = pool->head;
    while (cur != NULL) {
        pmalloc_page_header_t *next = cur->next;
        pmalloc_free_page(cur, cur->size);
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
    if(pool == NULL) {
        return;
    }
    // Lock
    pmalloc_lock_mutex(&pool->mutex);

    // Traverse the linked list, marking all the pages. Make sure we don't write
    // to a page once it's marked.
    pmalloc_page_header_t *cur = pool->head;
    while (cur != NULL) {
        cur->ro = true;
        pmalloc_markro_page(cur, cur->size);
        cur = cur->next;
    }

    // Unlock
    pmalloc_unlock_mutex(&pool->mutex);
}


static void pmalloc_new_page(pmalloc_pool_t *pool);

void *pmalloc_align(pmalloc_pool_t *pool, size_t size, size_t align);
