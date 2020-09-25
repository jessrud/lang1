#pragma once

#include <stddef.h>

typedef struct SrcPosn {
    size_t col;
    size_t row;
} SrcPosn;

typedef struct SrcSpan {
    char *docName;
    SrcPosn start;
    SrcPosn end;
} SrcSpan;
