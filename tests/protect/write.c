// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>
#include <stdlib.h>
#include <signal.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"

void segv_handler(int signal) {
    assert(signal == SIGSEGV);
    exit(0);
}


int main(void) {
    signal(SIGSEGV, segv_handler);

    pmalloc_pool_t *pool = pmalloc_create_pool();
    char *x = pmalloc(pool, 1);

    pmalloc_protect_pool(pool);
    *x = 'A';

    assert(false);
}
