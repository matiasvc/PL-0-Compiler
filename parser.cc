#include "parser.h"

#include <cstdio>

#include "global.h"
#include "lexer.h"
#include "util.h"


void next() {
  g.type = lex();
  ++g.raw;
}

void expect(int match) {
  if (g.type != match) {
    error("syntax error");
  }

  next();
}

void block() {
  if (g.depth++ > 1) {
    error("nesting depth exceeded");
  }

  if (g.type == TOK_CONST) {
    expect(TOK_CONST);
    expect(TOK_IDENT);
    expect(TOK_EQUAL);
    expect(TOK_NUMBER);
    while (g.type == TOK_COMMA) {
      expect(TOK_COMMA);
      expect(TOK_IDENT);
      expect(TOK_EQUAL);
      expect(TOK_NUMBER);
    }
    expect(TOK_SEMICOLON);
  }

  if (g.type == TOK_VAR) {
    expect(TOK_VAR);
    expect(TOK_IDENT);
    while (g.type == TOK_COMMA) {
      expect(TOK_COMMA);
      expect(TOK_IDENT);
    }
    expect(TOK_SEMICOLON);
  }

  while (g.type == TOK_PROCEDURE) {
    expect(TOK_PROCEDURE);
    expect(TOK_IDENT);
    expect(TOK_SEMICOLON);

    block();

    expect(TOK_SEMICOLON);
  }

  statement();

  if (--g.depth < 0) {
    error("nesting depth fell below 0");
  }
}

void statement() {
  switch (g.type) {
    case TOK_IDENT:
      expect(TOK_IDENT);
      expect(TOK_ASSIGN);
      expression();
      break;
    case TOK_CALL:
      expect(TOK_CALL);
      expect(TOK_IDENT);
      break;
    case TOK_BEGIN:
      expect(TOK_BEGIN);
      statement();
      while (g.type == TOK_SEMICOLON) {
        expect(TOK_SEMICOLON);
        statement();
      }
      expect(TOK_END);
      break;
    case TOK_IF:
      expect(TOK_IF);
      condition();
      expect(TOK_THEN);
      statement();
      break;
    case TOK_WHILE:
      expect(TOK_WHILE);
      condition();
      expect(TOK_DO);
      statement();
      break;
  }
}

void condition() {
  if (g.type == TOK_ODD) {
    expect(TOK_ODD);
    expression();
  } else {
    expression();

    switch (g.type) {
      case TOK_EQUAL:
      case TOK_HASH:
      case TOK_LESSTHAN:
      case TOK_GREATERTHAN:
        next();
        break;
      default:
        error("invalid conditional");
    }

    expression();
  }
}

void factor() {
  switch (g.type) {
    case TOK_IDENT:
    case TOK_NUMBER:
      next();
      break;
    case TOK_LPAREN:
      expect(TOK_LPAREN);
      expression();
      expect(TOK_RPAREN);
  }
}

void term() {
  factor();

  while (g.type == TOK_MULTIPLY || g.type == TOK_DIVIDE) {
    next();
    factor();
  }
}

void expression() {
  if (g.type == TOK_PLUS || g.type == TOK_MINUS) {
    next();
  }

  term();

  while (g.type == TOK_PLUS || g.type == TOK_MINUS) {
    next();
    term();
  }
}

void parse() {
  next();
  block();
  expect(TOK_DOT);

  if (g.type != 0) {
    error("extra tokens at the end of file");
  }
}
