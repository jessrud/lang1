#include "parser.h"
#include "ast.h"
#include "common/macros.h"
#include "gendef.h"
#include "lexer.h"
#include <string.h>

static bool advance(Parser *p);

bool Parser_init(Parser *p, char *docName, Lexer *lex) {
    *p = (Parser){
        .inputName = docName,
        .lex = lex,
        .cur = (TokContext){0},
        .next = (TokContext){0},
        .err = (ParseError){0},
    };
    ByteBuf_init(&p->cur.valueBuf, 20);
    ByteBuf_init(&p->next.valueBuf, 20);

    return advance(p) && advance(p);
}

void Parser_cleanup(Parser *p) {
    ByteBuf_free(&p->cur.valueBuf);
    ByteBuf_free(&p->next.valueBuf);
    *p = (Parser){0};
}

static bool advance(Parser *p) {
    // do nothing if we have already errored (for now)
    if ERROR (p->cur.tok)
        return false;

    Token tok = Lexer_next(p->lex);

    ByteBuf curBuf = p->cur.valueBuf;
    p->cur = p->next;

    p->next = (TokContext){
        .tok = tok,
        .span =
            {
                .docName = p->inputName,
                .start = p->lex->startPosn,
                .end = p->lex->curPosn,
            },
        .valueBuf = curBuf,
        .value = NULL,
    };

    // copy over token value if it exists.
    if (p->lex->tokenValue != NULL) {
        ByteBuf_copy(&p->next.valueBuf, &p->lex->valueBuf);
        p->next.value = p->next.valueBuf.data;
    }

    if ERROR (p->cur.tok) {
        p->err = (ParseError){
            .span = p->cur.span,

            .type = ParseError_lexError,
            .lexError = p->cur.tok,
        };
        return false;
    }
    return true;
}

#ifdef TESTING
#include <stdio.h>

int main() {
    TestLexer_ReadCtx rctx;
    Lexer lex;
    Lexer_init(&lex);
    TestLexer_init(rctx, lex, "three words here");

    Parser parser;
    Parser_init(&parser, "(test)", &lex);

    printf("current val: %s\n", parser.cur.value);
    printf("next val: %s\n", parser.next.value);

    advance(&parser);

    printf("current val: %s\n", parser.cur.value);
    printf("next val: %s\n", parser.next.value);

    advance(&parser);

    assert(parser.next.tok == EOF);
    assert(parser.next.value == NULL);

    Parser_cleanup(&parser);
    Lexer_cleanup(&lex);
}

#endif
