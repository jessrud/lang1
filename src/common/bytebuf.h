#pragma once

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    char *data;
    size_t capacity;
    size_t len;
} ByteBuf;

void ByteBuf_init(ByteBuf *self, size_t capacity);
void ByteBuf_ensure(ByteBuf *self, size_t capacity);
void ByteBuf_append(ByteBuf *self, char b);
void ByteBuf_copy(ByteBuf *dest, const ByteBuf *src);
void ByteBuf_free(ByteBuf *self);
char *ByteBuf_string(ByteBuf *self);
