// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>
#include <stdlib.h>

#include "pmalloc/internals.h"

// This file should only be compiled on Linux.
#if !defined(PMALLOC_LINUX)
#   error "This file contains code specific to Linux"
#endif


void *pmalloc_alloc_pool(void) {
    void *ret = malloc(sizeof(pmalloc_pool_t));
    assert(ret);
    return ret;
}

void pmalloc_free_pool(void *ptr) {
    assert(ptr);
    free(ptr);
}
