// a streaming lexer for codename lang1. see `lexer#read_char` and
// `lexer#data` for how to overload the input source.

#pragma once

#include "common/bytebuf.h"
#include "gendef.h"
#include <stddef.h>

// result type of `lexer_next()`, provides
// space for EOF and context specific
// errors that `lexer#read_char()` might
// produce.

typedef enum Token {
    Token_unexpected = -500,
    Token_ident = 0,
    Token_decLit,

    Token_colon,

    Token_lParen,
    Token_rParen,
    Token_lBrace,
    Token_rBrace,
    Token_semi,
    Token_assign,
    Token_comma,

    Token_keywordStart,
    // keywords
    Token_if,
    Token_return,
    Token_goto,
    Token_fn,
    Token_const,
    Token_var,
    Token_val,
    Token_export,
    Token_void,
    Token_int,
    Token_bool,
    Token_true,
    Token_false,
    Token_ptr,
    Token_keywordEnd,

    Token_boolAnd,
    Token_boolOr,
    Token_boolNot,

    Token_eq,
    Token_nEq,
    Token_gtEq,
    Token_ltEq,
    Token_gt,
    Token_lt,

    // arithmetic

    Token_add,
    Token_sub,
    Token_mul,
    Token_div,

    // binary arithmetic
    Token_binOr,
    Token_binAnd,
    Token_binNot,
    Token_xOr,
    Token_xAnd,
    Token_rShift,
    Token_lShift,
} Token;

typedef struct Lexer {
    // optional context passed into `lexer#read_char` when calling
    // `lexer_next(lexer)`
    void *context;

    int curChar;
    int nextChar;

    // the running total characters that the lexer has consumed.
    size_t consumed;

    // the running total tokens that the lexer has produced.
    size_t produced;

    ByteBuf valueBuf;

    // an optional string of data associated with the last token the lexer
    // produced. invalidated after next call to `lexer_next()`, `lexer_init()`,
    // or `lexer_cleanup()`
    char *tokenValue;

    // the begining of the last analyzed span of text.
    SrcPosn startPosn;

    // the position of the last character consumed and analyzed by the lexer
    SrcPosn curPosn;

    // a user provided function for getting the next character of input for the
    // lexer to analyze
    int (*readChar)(void *);
} Lexer;

void Lexer_init(Lexer *);
void Lexer_cleanup(Lexer *);
Token Lexer_next(Lexer *);

#ifdef TESTING

// some helper definitions for writing tests involving the lexer

#include <stdio.h>

#define TestLexer_init(state, lexer, input)                                    \
    state.data = &input[0];                                                    \
    state.len = (sizeof input) - 1;                                            \
    state.offset = 0;                                                          \
    lexer.context = (void *)&state;                                            \
    lexer.readChar = (int (*)(void *))TestLexer_readChar

typedef struct TestLexer_ReadCtx {
    const char *data;
    size_t len;
    size_t offset;
} TestLexer_ReadCtx;

static int TestLexer_readChar(TestLexer_ReadCtx *state) {
    if (state->offset < state->len) {
        char ch = state->data[state->offset];
        state->offset += 1;
        return ch;
    }
    return EOF;
}
#endif
