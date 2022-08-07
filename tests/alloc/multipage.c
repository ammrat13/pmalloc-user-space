// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>
#include <stdint.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"


int main(void) {
    pmalloc_pool_t *pool = pmalloc_create_pool();

    char *x = pmalloc_align(pool, PMALLOC_DEFAULT_PAGESIZE/2 + 1, 0);
    char *y = pmalloc_align(pool, PMALLOC_DEFAULT_PAGESIZE/2 + 1, 0);

    assert(pool->head);
    assert(pool->head->next);
    assert(pool->head->next->next == NULL);

    char *z = pmalloc_align(pool, 1, 0);
    assert(z + 1 == y);
    assert(pool->head->next->next == NULL);

    pmalloc_destroy_pool(pool);
    return 0;
}
