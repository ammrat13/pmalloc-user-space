// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>

#include "pmalloc/pmalloc.h"
#include "pmalloc/internals.h"


int main(void) {
    pmalloc_pool_t *pool = pmalloc_create_pool();
    char *xs[5];

    for (unsigned char i = 0; i < 4; i++) {
        xs[i] = pmalloc(pool, 1);
        assert(pool->head != NULL);
        assert(pool->head->ro == false);

        pmalloc_page_header_t *cur = pool->head->next;
        while (cur != NULL) {
            assert(cur->ro == true);
            cur = cur->next;
        }

        *xs[i] = 'A' + i;

        for (unsigned char j = 0; j < i; j++) {
            assert(*xs[j] == 'A' + j);
        }
    }

    pmalloc_destroy_pool(pool);
    return 0;
}
