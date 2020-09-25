#pragma once

#include <stdio.h>
#include <stdlib.h>

#define CSI "\x1B["
#define CLEAR CSI "0m"
#define BOLD CSI "1m"
#define BLINK CSI "5m"
#define UL CSI "4m"
#define RED CSI "31m"

#ifndef NDEBUG
#define assert(e)                                                              \
    if (!(e)) {                                                                \
        fprintf(stderr, "%s:%d: assertion '%s' failed in function '%s'\n",     \
                __FILE__, __LINE__, #e, __func__);                             \
        exit(1);                                                               \
    }                                                                          \
    ((void)0)
#else
#define assert(e) ((void)0)
#endif

#define ERROR(e) ((e) < 0)
