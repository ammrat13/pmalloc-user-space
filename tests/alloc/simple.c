// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>
#include <stdint.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"


int main(void) {
    pmalloc_pool_t *pool = pmalloc_create_pool();
    volatile char *x = pmalloc_align(pool, 7, 0);

    assert(pool->head != NULL);
    assert(pool->head->page_size == PMALLOC_DEFAULT_PAGESIZE);
    assert(pool->head->ro == false);

    assert(pool->head->bp_offset == (ptrdiff_t) x - (ptrdiff_t) pool->head);
    assert(pool->head->bp_offset == PMALLOC_DEFAULT_PAGESIZE - 7);

    for(size_t i = 0; i < 7; i++) {
        x[i] = 'A' + i;
    }
    for(size_t i = 0; i < 7; i++) {
        assert(x[i] == 'A' + i);
    }

    pmalloc_destroy_pool(pool);
    return 0;
}
