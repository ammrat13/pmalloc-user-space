// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"


int main(void) {
    pmalloc_pool_t *pool = pmalloc_create_pool();
    pmalloc_align(pool, 7, 4);

    assert(pool->head);
    assert(pool->head->bp_offset == PMALLOC_DEFAULT_PAGESIZE - 16);

    pmalloc_destroy_pool(pool);
    return 0;
}
