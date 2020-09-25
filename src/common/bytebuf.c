#include "bytebuf.h"
#include "macros.h"
#include <assert.h>
#include <stddef.h>
#include <string.h>

void ByteBuf_init(ByteBuf *self, size_t capacity) {
    self->len = 0;
    self->capacity = capacity;
    self->data = malloc(capacity * sizeof(char *));
}

void ByteBuf_ensure(ByteBuf *self, size_t capacity) {
    if (self->capacity < capacity) {
        self->data = realloc(self->data, capacity);
        self->capacity = capacity;
    }
}

void ByteBuf_append(ByteBuf *self, char byte) {
    if (self->len == self->capacity) {
        self->capacity *= 2;
        self->data = realloc(self->data, self->capacity * sizeof(char *));
    }
    self->data[self->len++] = byte;
}

void ByteBuf_appendArr(ByteBuf *self, const char *array, size_t arrayLen) {
    const size_t start = self->len;
    self->len += arrayLen;
    if (self->capacity < self->len) {
        self->data = realloc(self->data, self->len);
        self->capacity = self->len;
    }

    for (size_t i = 0; i < arrayLen; i++)
        self->data[i + start] = array[i];
}

void ByteBuf_appendBuf(ByteBuf *self, const ByteBuf *other) {
    ByteBuf_appendArr(self, other->data, other->len);
}

char *ByteBuf_string(ByteBuf *self) {
    assert(self->len > 0);
    if (self->data[self->len - 1] != 0)
        ByteBuf_append(self, 0);
    return self->data;
}

void ByteBuf_copy(ByteBuf *dest, const ByteBuf *src) {
    ByteBuf_ensure(dest, src->capacity);
    memcpy(dest->data, src->data, src->len);
}

void ByteBuf_free(ByteBuf *self) {
    free(self->data);
    self->data = NULL;
    self->len = 0;
    self->capacity = 0;
}

#ifdef TESTING

#include "macros.h"
#include <stdio.h>

void test_alloc() {
    ByteBuf buf;
    ByteBuf_init(&buf, 10);

    assert(buf.capacity == 10);
    assert(buf.len == 0);
    assert(buf.data != NULL);

    ByteBuf_free(&buf);

    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.data == NULL);
}

void test_append() {
    ByteBuf buf;
    ByteBuf_init(&buf, 10);

    for (int i = 0; i < 100; i++) {
        ByteBuf_append(&buf, 'c');
    }

    for (int i = 0; i < 100; i++) {
        assert(buf.data[i] == 'c');
    }

    ByteBuf_free(&buf);
    assert(buf.capacity == 0);
    assert(buf.len == 0);
    assert(buf.data == NULL);
}

int main() {
    printf("bytebuf alloc...");
    test_alloc();
    printf("OK!\n");
    printf("bytebuf append...");
    test_append();
    printf("OK!\n");
}

#endif
