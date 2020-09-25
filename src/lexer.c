#include "lexer.h"
#include "common/bytebuf.h"
#include "common/macros.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// textual representation of token classes
const char *token_rep[] = {
    [Token_ident] = "(ident)", [Token_decLit] = "(decimal_lit)",
    [Token_colon] = ":",       [Token_lParen] = "(",
    [Token_rParen] = ")",      [Token_lBrace] = "{",
    [Token_rBrace] = "}",      [Token_semi] = ";",
    [Token_comma] = ",",       [Token_assign] = "=",

    [Token_if] = "if",         [Token_return] = "return",
    [Token_goto] = "goto",   [Token_fn] = "fn",
    [Token_export] = "export", [Token_const] = "const",
    [Token_var] = "var",       [Token_val] = "val",
    [Token_void] = "void",     [Token_int] = "int",
    [Token_bool] = "bool",     [Token_true] = "true",
    [Token_false] = "false",   [Token_ptr] = "ptr",

    [Token_boolAnd] = "&&",    [Token_boolOr] = "||",
    [Token_boolNot] = "!",

    [Token_eq] = "==",         [Token_nEq] = "!=",
    [Token_gtEq] = ">=",       [Token_ltEq] = "<=",
    [Token_rShift] = ">>",     [Token_lShift] = "<<",

    [Token_gt] = ">",          [Token_lt] = "<",

    [Token_add] = "+",         [Token_sub] = "-",
    [Token_mul] = "*",         [Token_div] = "/",

    [Token_binOr] = "|",       [Token_binAnd] = "&",
    [Token_binNot] = "~",      [Token_xOr] = "~|",
    [Token_xAnd] = "~&",
};

int stdin_readChar(void *_) {
    (void)_;
    return getchar();
}

static void basicInit(Lexer *lex) {
    *lex = (Lexer){0};
    lex->readChar = stdin_readChar;
}

void Lexer_init(Lexer *lex) {
    basicInit(lex);
    ByteBuf_init(&lex->valueBuf, 32);
}

void Lexer_cleanup(Lexer *lex) {
    ByteBuf_free(&lex->valueBuf);
    basicInit(lex);
}

static bool advance(Lexer *lex) {
    if (lex->consumed == 0) {
        lex->curChar = lex->readChar(lex->context);
    } else
        lex->curChar = lex->nextChar;

    lex->nextChar = lex->readChar(lex->context);

    if (lex->curChar <= EOF) {
        lex->tokenValue = NULL;
        return false;
    } else
        lex->consumed += 1;

    if (lex->curChar == '\n') {
        lex->curPosn.col = 0;
        lex->curPosn.row += 1;
    }

    // only increment the column after the first
    // character is consumed
    if (lex->consumed > 1)
        lex->curPosn.col += 1;

    return true;
}

// alpha-numeric sequence (possibly starting with _)
static Token word(Lexer *lex) {
    while (isalnum(lex->nextChar) || lex->nextChar == '_') {
        advance(lex);
        ByteBuf_append(&lex->valueBuf, (char)lex->curChar);
    }
    lex->tokenValue = ByteBuf_string(&lex->valueBuf);

    for (int TOK = Token_keywordStart + 1; TOK < Token_keywordEnd; TOK++) {
        if (!strcmp(token_rep[TOK], lex->tokenValue)) {
            lex->tokenValue = NULL;
            return TOK;
        }
    }

    return Token_ident;
}

// decimal integer literals
static Token number(Lexer *lex) {
    while (isdigit(lex->nextChar)) {
        advance(lex);
        ByteBuf_append(&lex->valueBuf, (char)lex->curChar);
    }
    lex->tokenValue = ByteBuf_string(&lex->valueBuf);
    return Token_decLit;
}

// get next token of input
Token Lexer_next(Lexer *lex) {
    lex->valueBuf.len = 0;
// lexer_cur_char will be an error if !advance();
advance:
    if (!advance(lex))
        return (Token)lex->curChar;

    if (isspace(lex->curChar))
        goto advance;

    // assume successful token production here.
    // we undo this if execution falls through to the
    // end of the function, or fails mid-token
    lex->produced += 1;
    lex->startPosn.col = lex->curPosn.col;
    lex->startPosn.row = lex->curPosn.row;
    ByteBuf_append(&lex->valueBuf, (char)lex->curChar);

    //// Dynamic Tokens

    // identifiers
    if (isalpha(lex->curChar) || lex->curChar == '_')
        return word(lex);

    // int literals
    if (isdigit(lex->curChar))
        return number(lex);

        //// Static Tokens

// helper for two character static tokens
#define succ_if_next(c, TOKEN)                                                 \
    if (lex->nextChar == c) {                                                  \
        advance(lex);                                                          \
        return TOKEN;                                                          \
    }                                                                          \
    ((void)0)

    lex->tokenValue = NULL;
    switch (lex->curChar) {

    // sequence and block tokens
    case '(':
        return Token_lParen;
    case ')':
        return Token_rParen;
    case '{':
        return Token_lBrace;
    case '}':
        return Token_rBrace;
    case ';':
        return Token_semi;
    case ',':
        return Token_comma;

    // operators
    case ':':
        return Token_colon;

    case '+':
        return Token_add;
    case '-':
        return Token_sub;
    case '*':
        return Token_mul;
    case '/':
        return Token_div;

    case '!':
        succ_if_next('=', Token_nEq);
        return Token_boolNot;

    case '>':
        succ_if_next('=', Token_gtEq);
        succ_if_next('>', Token_rShift);
        return Token_gt;

    case '<':
        succ_if_next('<', Token_lShift);
        succ_if_next('=', Token_ltEq);
        return Token_lt;

    case '=':
        succ_if_next('=', Token_eq);
        return Token_assign;

    case '|':
        succ_if_next('|', Token_boolOr);
        return Token_binOr;
    case '&':
        succ_if_next('&', Token_boolAnd);
        return Token_binAnd;
    case '~':
        succ_if_next('&', Token_xAnd);
        succ_if_next('|', Token_xOr);
        return Token_binNot;
    }

    lex->produced -= 1;
    return Token_unexpected;
}

#ifdef TESTING

void test_basic();

int main() { test_basic(); }

void test_basic() {
    Lexer lex;
    TestLexer_ReadCtx rctx;

    Lexer_init(&lex);
    TestLexer_init(rctx, lex, "hello\n there");

    Token token = Lexer_next(&lex);

    assert(token == Token_ident);
    assert(lex.consumed == strlen("hello"));
    assert(lex.tokenValue != NULL);
    assert(!strcmp(lex.tokenValue, "hello"));

    token = Lexer_next(&lex);

    assert(token == Token_ident);
    assert(lex.tokenValue != NULL);
    assert(!strcmp(lex.tokenValue, "there"));

    token = Lexer_next(&lex);

    assert(token == EOF);
    assert(lex.produced == 2);
    assert(lex.consumed == rctx.len);

    Lexer_cleanup(&lex);
    Lexer_init(&lex);
    TestLexer_init(rctx, lex, "if (true) <=");

    assert(Lexer_next(&lex) == Token_if);
    assert(Lexer_next(&lex) == Token_lParen);
    assert(Lexer_next(&lex) == Token_true);
    assert(Lexer_next(&lex) == Token_rParen);
    assert(Lexer_next(&lex) == Token_ltEq);

    Lexer_cleanup(&lex);
}

#endif
