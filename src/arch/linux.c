// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "pmalloc/internals.h"

#if \
    defined(PMALLOC_ROUND_PAGESIZE) && \
    !defined(PMALLOC_PAGESIZE_HARDCODED) && \
    defined(PMALLOC_HUGETLB)
#   include <stdio.h>
#   include <string.h>
#endif

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


void *pmalloc_alloc_page(size_t *length) {
    assert(length);

    // If we have to round, do so here. The result is stored in `*length` for
    // integration with the code later.
    #if defined(PMALLOC_ROUND_PAGESIZE)
        // The page size we'll use for rounding. Try to compute statically, and
        // failing that, only compute it once.
        static size_t page_size =
        #if defined(PMALLOC_PAGESIZE_HARDCODED)
            PMALLOC_PAGESIZE;
        #else
            0;
            if (page_size == 0) {
                #if defined(PMALLOC_HUGETLB)
                    // Doesn't seem to be a way to get huge page size
                    // programmatically. Have to use `/proc/meminfo`.
                    FILE *f = fopen("/proc/meminfo", "r");
                    assert(f);
                    // Read the entire file into RAM
                    char *fdata;
                    {
                        fdata = calloc(PMALLOC_MEMINFO_MAXSIZE + 1, 1);
                        fread(fdata, 1, PMALLOC_MEMINFO_MAXSIZE, f);
                        assert(feof(f));
                    }
                    // Get the start of the line
                    char *find;
                    {
                        find = strstr(fdata, "Hugepagesize:");
                        assert(find);
                    }
                    // Increment to the data
                    find += strlen("Hugepagesize:");
                    while (*find == ' ')
                        find++;
                    // Set a null terminator at the next space
                    char *end;
                    {
                        end = find;
                        while (*end != ' ') {
                            assert('0' <= *end && *end <= '9');
                            end++;
                        }
                        *end = 0;
                    }
                    // Assert we have the right units
                    assert(end[1] == 'k');
                    assert(end[2] == 'B');
                    assert(end[3] == '\n');
                    // Get the size
                    page_size = atoi(find) * 1024;
                    // Free the buffer
                    free(fdata);
                #else
                    page_size = sysconf(_SC_PAGE_SIZE);
                #endif
            }
        #endif
        // Sanity checks for page size. This might be a bit too aggressive, but
        // that's fine.
        assert(
            page_size == 4096 ||
            page_size == 2097152 ||
            page_size == 1073741824);
        // Round up the length
        *length /= page_size;
        *length *= page_size;
        *length += page_size;
    #endif

    void *ret = mmap(
        NULL, *length,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | PMALLOC_HUGETLB_FLAGS,
        -1, 0);
    assert(ret != MAP_FAILED);
    return ret;
}

void pmalloc_free_page(void *ptr, size_t length) {
    int ret = munmap(ptr, length);
    assert(ret == 0);
}

void pmalloc_markro_page(void *ptr, size_t length) {
    int ret =  mprotect(ptr, length, PROT_READ);
    assert(ret == 0);
}
