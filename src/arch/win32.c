// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <memoryapi.h>

#include "pmalloc/internals.h"

#if defined(PMALLOC_ROUND_PAGESIZE)
#   include <sysinfoapi.h>
#endif

// This file should only be compiled on Windows.
#if !defined(PMALLOC_WIN32)
#   error "This file contains code specific to Windows"
#endif


void* pmalloc_alloc_pool(void) {
    void* ret = malloc(sizeof(pmalloc_pool_t));
    assert(ret);
    return ret;
}

void pmalloc_free_pool(void* ptr) {
    assert(ptr);
    free(ptr);
}


void* pmalloc_alloc_page(size_t* size) {
    assert(size);
    assert(*size > 0);

    // Get the page sizes if we have to.
    #if defined(PMALLOC_ROUND_PAGESIZE)
        static size_t page_size = 0;
        if (page_size == 0) {
            SYSTEM_INFO sysinfo_ret;
            GetSystemInfo(&sysinfo_ret);
            page_size = sysinfo_ret.dwPageSize;
            assert(page_size != 0);
            #if defined(PMALLOC_AGGRESSIVE_PAGESIZE_CHECKS)
                assert(page_size == 4096);
            #endif
        }
    #endif

    // Compute size
    #if defined(PMALLOC_ROUND_PAGESIZE)
        *size = pmalloc_round_up(*size, page_size);
    #endif

    // Allocate
    LPVOID ret = VirtualAlloc(
        NULL, *size,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE);
    assert(ret);
    return ret;
}

void pmalloc_free_page(void* ptr, size_t size) {
    assert(ptr);
    assert(size > 0);
    bool ret = VirtualFree(ptr, 0, MEM_RELEASE);
    assert(ret);
}

void pmalloc_markro_page(void* ptr, size_t size) {
    assert(ptr);
    assert(size > 0);
    DWORD old_protect;
    bool ret = VirtualProtect(ptr, size, PAGE_READONLY, &old_protect);
    assert(ret);
    assert(old_protect == PAGE_READWRITE);
}
