#pragma once

#include "ast.h"
#include "common/bytebuf.h"
#include "gendef.h"
#include "lexer.h"

typedef struct Parser Parser;
typedef struct ParseError ParseError;
typedef struct TokContext TokContext;

struct ParseError {
    SrcSpan span;
    enum { ParseError_lexError } type;
    union {
        Token lexError;
    };
};

struct TokContext {
    Token tok;
    SrcSpan span;
    char *value;
    ByteBuf valueBuf;
};

struct Parser {
    char *inputName;
    Lexer *lex;

    TokContext cur;
    TokContext next;

    ParseError err;
};
