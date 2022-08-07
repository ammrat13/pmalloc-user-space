// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"


int main(void) {
    pmalloc_pool_t *pool = pmalloc_create_pool();
    char *x = pmalloc(pool, 1);

    size_t page_size_before = pool->head->page_size;
    size_t bp_offset_before = pool->head->bp_offset;

    *x = 'A';

    pmalloc_protect_pool(pool);
    assert(pool->head->next == NULL);
    assert(pool->head->page_size == page_size_before);
    assert(pool->head->bp_offset == bp_offset_before);
    assert(pool->head->ro == true);

    assert(*x == 'A');

    pmalloc_destroy_pool(pool);
    return 0;
}
