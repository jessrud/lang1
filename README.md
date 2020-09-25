# codename lang1 [WIP]
a relatively simple c-like programming language

## Why?
Mainly, just because I want to. 

## General goals
* easy to parse
* easy to analyze
* easy to generate efficient code for
* non-ambiguous type/constant expression contexts.
* suitable as a core for a larger and more intricate systems programming 
  language

grammar target:
```bnf
program := topDecl+

topDecl := ?'export' genDecl
         | ?'export' 'fn' IDENT '(' list( IDENT ':' type , ',') ')' type '{' stmt* '}'

genDecl := 'const' IDENT ':' type '=' expr ';'
         | 'var' IDENT ':' type '=' expr ';'

stmt := expr ';'
      | genDecl
      | 'return' expr? ';'
      | IDENT : statement
      | 'goto' IDENT ';'
      | 'if' '(' expr ')' (stmt | '{' *stmt '}')

expr := '(' expr ')'
      | expr BINOP expr
      | 'ptr IDENT
      | 'val expr
      | expr ':' type
      | expr '(' list(expr,',') ')'
      | INT
      | BOOL

type :=  'void'
      || 'bool'
      || 'int'
      || 'ptr' type 
      || 'const' type
      || '(' type ')'

list(t,s) := t (*(t s) ?t)
IDENT := '_' ('a'..'z' | 'A'..'Z' | '0'..'9')*
BINOP := '+' | '-' | '*' | '/' | '==' | '>' | '<' | ...
INT := - +[0-9]
BOOL := 'true' | 'false'
```
Things of note:
* very basic control flow: labels, goto, and if.
* type system is initially non-extensible.
* pointer ref/deref with with 'addr name' or 'val expr' respectively. 

## Status
   
   The lexer is mostly done, and I am currently working on the parsing
architecture. The lexer operates on one character of lookahead, and the parser
will operate on one token of lookahead.


I am also still working out the build system and testing infrastructure as well.
you might take a look at the 'batre' build script.


