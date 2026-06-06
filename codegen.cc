#include "codegen.h"

#include "lexer.h"
#include "util.h"
#include "global.h"

#define PL0C_VERSION "1.0.0"

void cg_end() {
  aout("/* PL/0 compiler %s */\n", PL0C_VERSION);
}

void cg_const() {
  aout("const long %s=", g.token);
}

void cg_symbol() {
  switch (g.type) {
    case TOK_IDENT:
    case TOK_NUMBER:
      aout("%s", g.token);
      break;
    case TOK_BEGIN:
      aout("{\n");
      break;
    case TOK_END:
      aout(";\n}\n");
      break;
    case TOK_IF:
      aout("if(");
      break;
    case TOK_THEN:
    case TOK_DO:
      aout(")");
      break;
    case TOK_ODD:
      aout("(");
      break;
    case TOK_WHILE:
      aout("while(");
      break;
    case TOK_EQUAL:
      aout("==");
      break;
    case TOK_COMMA:
      aout(",");
      break;
    case TOK_ASSIGN:
      aout("=");
      break;
    case TOK_HASH:
      aout("!=");
      break;
    case TOK_LESSTHAN:
      aout("<");
      break;
    case TOK_GREATERTHAN:
      aout(">");
      break;
    case TOK_PLUS:
      aout("+");
      break;
    case TOK_MINUS:
      aout("-");
      break;
    case TOK_MULTIPLY:
      aout("*");
      break;
    case TOK_DIVIDE:
      aout("/");
      break;
    case TOK_LPAREN:
      aout("(");
      break;
    case TOK_RPAREN:
      aout(")");
  }
}

void cg_semicolon() {
  aout(";\n");
}

void cg_var() {
  aout("long %s;\n", g.token);
}

void cg_crlf() {
  aout("\n");
}

void cg_procedure() {
  if (!g.proc) {
    aout("int main(int argc, char** argv) {\n");
  } else {
    aout("void %s() {\n", g.token);
  }
}

void cg_epilogue() {
  aout(";");

  if (!g.proc) {
    aout("return 0;");
  }

  aout("\n}\n\n");
}

