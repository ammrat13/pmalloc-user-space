// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>

#include "pmalloc/internals.h"

#if defined(PMALLOC_ROUND_PAGESIZE) && defined(PMALLOC_HUGETLB)
#   include <stdio.h>
#   include <string.h>
#   include <errno.h>
#endif

// This file should only be compiled on Linux.
#if !defined(PMALLOC_LINUX)
#   error "This file contains code specific to Linux"
#endif

// Sometimes, we only use a variable in an assert. Use this macro to mark them.
#if defined(NDEBUG)
#   define FOR_ASSERT(x) ((void) x)
#else
#   define FOR_ASSERT(x)
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


void *pmalloc_alloc_page(size_t *size) {
    assert(size);
    assert(*size > 0);
    void *ret;

    // Get the page sizes if we have to. If we're using huge pages, we might
    // need to fall back to normal pages.
    #if defined(PMALLOC_ROUND_PAGESIZE)
        static size_t page_size = 0;
        if (page_size == 0) {
            const ssize_t sysconf_ret = sysconf(_SC_PAGE_SIZE);
            assert(sysconf_ret != -1l);
            assert(sysconf_ret != 0l);
            page_size = sysconf_ret;
            #if defined(PMALLOC_AGGRESSIVE_PAGESIZE_CHECKS)
                assert(page_size == 4096);
            #endif
        }
    #   if defined(PMALLOC_HUGETLB)
            static size_t huge_page_size = 0;
            if (huge_page_size == 0) {
                // Doesn't seem to be a way to get huge page size
                // programmatically. Have to use `/proc/meminfo`.
                FILE *f = fopen(PMALLOC_PROC_MOUNT "/meminfo", "r");
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
                    find = strstr(fdata, PMALLOC_MEMINFO_HUGEPAGE);
                    assert(find);
                }
                // Increment to the data
                find += strlen(PMALLOC_MEMINFO_HUGEPAGE);
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
                huge_page_size = atoi(find) * 1024;
                assert(huge_page_size != 0);
                assert(huge_page_size > page_size);
                #if defined(PMALLOC_AGGRESSIVE_PAGESIZE_CHECKS)
                    assert(huge_page_size == 2097152);
                #endif
                // Free everything
                free(fdata);
                fclose(f);
            }
    #   endif
    #endif

    // Compute the size to allocate in each page
    #if defined(PMALLOC_ROUND_PAGESIZE)
        const size_t size_page = pmalloc_round_up(*size, page_size);
    #   if defined(PMALLOC_HUGETLB)
            const size_t size_huge_page =
                pmalloc_round_up(*size, huge_page_size);
    #   endif
    #endif

    // Try HugeTLB
    #if defined(PMALLOC_HUGETLB)
        ret = mmap(
            NULL, size_huge_page,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
            -1, 0);
        assert(ret != MAP_FAILED || errno == ENOMEM);
        // If we succeed, great. If not, carry on.
        if (ret != MAP_FAILED) {
            *size = size_huge_page;
            return ret;
        }
    #endif

    // Round up the page size if needed
    #if defined(PMALLOC_ROUND_PAGESIZE)
        *size = size_page;
    #endif
    // Do the allocation
    ret = mmap(
        NULL, *size,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS,
        -1, 0);
    FOR_ASSERT(ret);
    assert(ret != MAP_FAILED);
    return ret;
}

void pmalloc_free_page(void *ptr, size_t size) {
    assert(ptr);
    assert(size > 0);
    int ret = munmap(ptr, size);
    FOR_ASSERT(ret);
    assert(ret == 0);
}

void pmalloc_markro_page(void *ptr, size_t size) {
    assert(ptr);
    assert(size > 0);
    int ret =  mprotect(ptr, size, PROT_READ);
    FOR_ASSERT(ret);
    assert(ret == 0);
}


#if defined(PMALLOC_THREADS)
#   if defined(PMALLOC_PTHREADS)

void pmalloc_alloc_mutex(pmalloc_mutex_t *mutex) {
    assert(mutex);
    int ret;
    FOR_ASSERT(ret);

    #if defined(NDEBUG)
        ret = pthread_mutex_init(mutex, NULL);
        // Asserts don't do anything in here.
    #else
        // If we're building in debug mode, add extra instrumentation to the
        // mutex to detect errors.
        pthread_mutexattr_t attr;
        ret = pthread_mutexattr_init(&attr);
        assert(ret == 0);
        ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
        assert(ret == 0);
        ret = pthread_mutex_init(mutex, &attr);
        assert(ret == 0);
        ret = pthread_mutexattr_destroy(&attr);
        assert(ret == 0);
    #endif
}

void pmalloc_free_mutex(pmalloc_mutex_t *mutex) {
    assert(mutex);
    int ret = pthread_mutex_destroy(mutex);
    FOR_ASSERT(ret);
    assert(ret == 0);
}

void pmalloc_lock_mutex(pmalloc_mutex_t *mutex) {
    assert(mutex);
    int ret = pthread_mutex_lock(mutex);
    FOR_ASSERT(ret);
    assert(ret == 0);
}

void pmalloc_unlock_mutex(pmalloc_mutex_t *mutex) {
    assert(mutex);
    int ret = pthread_mutex_unlock(mutex);
    FOR_ASSERT(ret);
    assert(ret == 0);
}

#   else
#       error "Linux does not support this threading library"
#   endif
#endif
