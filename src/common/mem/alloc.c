
#include "alloc.h"
#include "../macros.h"
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>

void *Mem_alloc(Alloc *alloc, size_t size) {
    return alloc->resize(alloc->context, NULL, size);
}
void *Mem_realloc(Alloc *alloc, void *addr, size_t newSize) {
    return alloc->resize(alloc, addr, newSize);
}
void Mem_free(Alloc *alloc, void *addr) { alloc->resize(alloc, addr, 0); }

// singleton implementation for malloc/realloc/free
void *malloc(size_t);
void *realloc(void *, size_t);
void free(void *);

void *cResize(void *_, void *addr, size_t newSize) {
    (void)_;

    void *newAddr = NULL;

    if (newSize == 0) {
        assert(addr != NULL);
        free(addr);

    } else if (addr == NULL) {
        void *newAddr = malloc(newSize);
        if (errno == ENOMEM)
            newAddr = NULL;

    } else {
        newAddr = realloc(addr, newSize);
        if (errno == ENOMEM)
            newAddr = NULL;
    }

    return newAddr;
}

Alloc mAlloc = {
    .context = NULL,
    .resize = cResize,
};

// Fixed Buffer Allocator
// example:
// char mem[100];
// Mem_FixedBuf fb = {.data = mem, .capacity = sizeof mem};
// Alloc fba = Alloc_fromFixedBuf(&fb);
Alloc FixedBufAlloc(FixedBuf *buf) {
    buf->top = 0;
    buf->last_alloc = NULL;
}
void *fbResize_(Mem_FixedBuf)
