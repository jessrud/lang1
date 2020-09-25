#pragma once

#include "../macros.h"
#include <stdbool.h>
#include <stdint.h>

// generic allocator
typedef struct Alloc {
    void *context;
    void *(*resize)(void *context, void *oldAddr, size_t);
} Alloc;

// global implementation for malloc
extern Alloc mAlloc;

typedef struct Mem_FixedBuf {
    void *data;
    size_t capacity;
    size_t top;
    void *last_alloc;
} FixedBuf;
Alloc Alloc_fromFixedBuf(FixedBuf *buf);

void *Mem_alloc(Alloc *alloc, size_t size);
void *Mem_realloc(Alloc *alloc, void *addr, size_t newSize);
void Mem_free(Alloc *alloc, void *addr);
