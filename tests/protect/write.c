// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"

#if defined(PMALLOC_LINUX) || defined(PMALLOC_WIN32)
#   include <stdlib.h>
#   include <signal.h>
    void segv_handler(int signal) {
        assert(signal == SIGSEGV);
        exit(0);
    }
#endif

int main(void) {
    #if defined(PMALLOC_LINUX) || defined(PMALLOC_WIN32)
        signal(SIGSEGV, segv_handler);
    #endif

    pmalloc_pool_t *pool = pmalloc_create_pool();
    char *x = pmalloc(pool, 1);

    pmalloc_protect_pool(pool);
    *x = 'A';

    return 1;
}
