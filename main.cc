#include <cstdio>
#include <cstdlib>

#include "global.h"
#include "lexer.h"
#include "util.h"

/*
 * pl0c -- PL/0 compiler.
 *
 * program = block "." .
 * block   = [ "const" ident "=" number { "," ident "=" number } ";" ]
 *           [ "var" ident { "," ident } ";" ]
 *           { "procedure" ident ";" block ";" } statement .
 * statement = [ ident ":=" expression
 *             | "call" ident
 *             | "begin" statement { ";" statement } "end"
 *             | "if" condition "then" statement
 *             | "while" condition "do" statement ] .
 * condition  = "odd" expression
 *              | expression ( "=" | "#" | "<" | ">" ) expression .
 * expression = [ "+" | "-" ] term { ( "+" | "-" ) term } .
 * term       = factor { ( "*" | "/" ) factor } .
 * factor     = ident
 *              | number
 *              | "(" expression ")" .
 */

/*** Parser ***/

static void parse() {
  while ((g.type = lex()) != 0) {
    ++g.raw;
    (void) fprintf(stdout, "%lu|%d\t", g.line, g.type);

    switch (g.type) {
      case TOK_IDENT:
      case TOK_NUMBER:
      case TOK_CONST:
      case TOK_VAR:
      case TOK_PROCEDURE:
      case TOK_CALL:
      case TOK_BEGIN:
      case TOK_END:
      case TOK_IF:
      case TOK_THEN:
      case TOK_WHILE:
      case TOK_DO:
      case TOK_ODD:
        (void) fprintf(stdout, "%s (%c)", g.token, g.type);
        break;
      case TOK_DOT:
      case TOK_EQUAL:
      case TOK_COMMA:
      case TOK_SEMICOLON:
      case TOK_HASH:
      case TOK_LESSTHAN:
      case TOK_GREATERTHAN:
      case TOK_PLUS:
      case TOK_MINUS:
      case TOK_MULTIPLY:
      case TOK_DIVIDE:
      case TOK_LPAREN:
      case TOK_RPAREN:
        (void) fputc(g.type, stdout);
        break;
      case TOK_ASSIGN:
        (void) fputs(":=", stdout);
    }
    (void) fputc('\n', stdout);
  }
}


/*** Main ***/

int main(int argc, char** argv) {

  char* startp;

  if (argc != 2) {
    (void) fputs("usage: pl0c file.pl0\n", stderr);
    exit(1);
  }

  readin(argv[1]);
  startp = g.raw;

  parse();

  free(startp);

  return 0;
}
