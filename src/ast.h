
#pragma once

#include "gendef.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct Ast_Expr Ast_Expr;
typedef struct Ast_Decl Ast_Decl;
typedef struct Ast_Module Ast_Module;
typedef struct Ast_Stmt Ast_Stmt;
typedef struct Ast_TypeExpr Ast_TypeExpr;

typedef struct Expr_BinOp Expr_BinOp;
typedef struct Expr_FnCall Expr_FnCall;
typedef struct Expr_Lit Expr_Lit;

typedef struct Stmt_Assign Stmt_Assign;
typedef struct Stmt_Label Stmt_Label;
typedef struct Stmt_If Stmt_If;

typedef struct Decl_Var Decl_Var;
typedef struct Decl_Fn Decl_Fn;

// Expression //////////////////////////////////////////////////////////////////
struct Expr_BinOp {
    enum {
        BinOp_plus,
        BinOp_minus,
    } type;

    Ast_Expr *left;
    Ast_Expr *right;
};

struct Expr_FnCall {
    Ast_Expr *head;
    size_t argc;
    Ast_Expr *argv;
};

struct Expr_Lit {
    enum { Lit_int, Lit_bool } type;

    union {
        size_t integer;
        bool boolean;
    };
};

struct Ast_Expr {
    enum {
        Expr_binOp,
        Expr_fnCall,
        Expr_ptr,
        Expr_val,
        Expr_asType,
        Expr_ident,
        Expr_lit
    } type;

    union {
        char *ident;
        Expr_BinOp *binOp;
        Expr_FnCall *fnCall;
        char *ptr;
        Ast_Expr *val;
        Expr_Lit *lit;
    };
};
// Statements //////////////////////////////////////////////////////////////////
struct Stmt_Assign {
    Ast_Expr *lvalue;
    Ast_Expr *rvalue;
};

struct Stmt_Label {
    char *name;
    Ast_Stmt *stmt;
};

struct Stmt_If {
    Ast_Expr *cond;
    Ast_Stmt *stmt;
};

struct Ast_Stmt {
    enum {
        Stmt_decl,
        Stmt_assign,
        Stmt_if,
        Stmt_return,
        Stmt_expr,
        Stmt_break,
        Stmt_label
    } type;

    union {
        Decl_Var *decl;
        Stmt_Assign *assign;
        Stmt_If *if_stmt;
        Ast_Expr *return_stmt;
        Ast_Expr *expr;
    };
};

// Type Expressions ////////////////////////////////////////////////////////////
struct Ast_TypeExpr {
    enum { TypeExpr_int, TypeExpr_bool, TypeExpr_ptr, TypeExpr_const } type;

    Ast_TypeExpr *inner;
};

// Declarations ////////////////////////////////////////////////////////////////
struct Decl_Var {
    bool is_const;
    Ast_TypeExpr *type;
    Ast_Expr *init;
};

struct Decl_Fn {
    char *name;
    size_t argc;
    Decl_Var *argv;
    size_t stmtc;
    Ast_Stmt *stmtv;
};

struct Ast_Decl {
    enum { Decl_var, Decl_fn } type;
    bool is_exported;
    union {
        Decl_Fn fn;
        Decl_Var var;
    };
};

struct Ast_Module {
    size_t declc;
    Ast_Decl *declv;
};
