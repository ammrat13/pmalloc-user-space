// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"


int main(void) {
    pmalloc_pool_t *pool = pmalloc_create_pool();

    for(size_t i = 0; i < 10; i++) {
        pmalloc_protect_pool(pool);
    }

    pmalloc_destroy_pool(pool);
    return 0;
}
