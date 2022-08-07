// SPDX-License-Identifier: GPL-2.0
// Copyright (C) 2022  Ammar Ratnani <ammrat13@gmail.com>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <stdlib.h>
#include <memoryapi.h>

#include "pmalloc/internals.h"

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
    assert(false);
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
