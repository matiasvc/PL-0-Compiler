#include <cstdio>
#include <cstdlib>

#include "global.h"
#include "parser.h"
#include "util.h"
#include "symtable.h"

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

/*** Main ***/

int main(int argc, char** argv) {

  char* startp;

  if (argc != 2) {
    (void) fputs("usage: pl0c file.pl0\n", stderr);
    exit(1);
  }

  readin(argv[1]);
  startp = g.raw;

  init_sym_table();

  parse();

  free(startp);

  return 0;
}
