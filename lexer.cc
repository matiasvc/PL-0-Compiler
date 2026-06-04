#include "lexer.h"

#include <cctype>
#include <climits>
#include <cstdlib>
#include <cstring>

#include <bsd/stdlib.h>

#include "global.h"
#include "util.h"

static void comment() {
  int ch;

  while ((ch = *g.raw++) != '}') {
    if (ch == '\0') {
      error("unterminated comment");
    }

    if (ch == '\n') {
      ++g.line;
    }
  }
}

// Identify the token pointet to by `g.raw`
static int ident() {
  char* p;
  size_t i;
  size_t len;

  // Save the current position of `g.raw` in `p`
  p = g.raw;

  // Loop through `g.raw` until we hit a character that is not alphanumeric or `_`
  while (isalnum(*g.raw) || *g.raw == '_') {
    ++g.raw;
  }

  // Get the size of our token
  len = g.raw - p;

  // Revert one step so `g.raw` points at the last character of the token, as
  // the caller increments `g.raw` after `lex()` returns
  --g.raw;

  // Allocate memory to copy the token string into
  free(g.token);
  if ((g.token = (char*)malloc(len + 1)) == NULL) {
    error("malloc failed");
  }

  // Copy token string into `g.token` and append a null-character
  for (i = 0; i < len; i++) {
    g.token[i] = *p++;
  }
  g.token[i] = '\0';

  // Detect the type of token
  if (!strcmp(g.token, "const")) {
    return TOK_CONST;
  } else if(!strcmp(g.token, "var")) {
    return TOK_VAR;
  } else if(!strcmp(g.token, "procedure")) {
    return TOK_PROCEDURE;
  } else if(!strcmp(g.token, "call")) {
    return TOK_CALL;
  } else if(!strcmp(g.token, "begin")) {
    return TOK_BEGIN;
  } else if(!strcmp(g.token, "end")) {
    return TOK_END;
  } else if(!strcmp(g.token, "if")) {
    return TOK_IF;
  } else if(!strcmp(g.token, "then")) {
    return TOK_THEN;
  } else if(!strcmp(g.token, "while")) {
    return TOK_WHILE;
  } else if(!strcmp(g.token, "do")) {
    return TOK_DO;
  } else if(!strcmp(g.token, "odd")) {
    return TOK_ODD;
  } else {
    // String did not match any keyword, so it must be an identifier
    return TOK_IDENT;
  }
}

// Parse the number pointed to by `g.raw`
static int number() {
  const char* errstr = NULL;
  char* p;
  size_t i;
  size_t j = 0;
  size_t len;

  p = g.raw;
  while (isdigit(*g.raw) || *g.raw == '_') {
    ++g.raw;
  }

  len = g.raw - p;
  --g.raw;

  free(g.token);
  if((g.token = (char*)malloc(len+1)) == NULL) {
    error("malloc failed");
  }

  for (i = 0; i < len; ++i) {
    if (isdigit(*p)) {
      g.token[j++] = *(p++);
    }
    g.token[j] = '\0';
  }

  (void) strtonum(g.token, 0, LONG_MAX, &errstr);
  if (errstr != NULL) {
    error("invalid number: %s", g.token);
  }

  return TOK_NUMBER;
}

// Lex the token pointed to by `g.raw`
int lex() {
again:
  // Skip any whitespace
  while(*g.raw == ' ' || *g.raw == '\t' || *g.raw == '\n') {
    if (*g.raw == '\n') {
      ++g.line;
    }
    ++g.raw;
  }

  if (isalpha(*g.raw) || *g.raw == '_') {
    return ident();
  }

  if (isdigit(*g.raw)) {
    return number();
  }

  switch (*g.raw) {
    case '{':
      comment();
      goto again;
    case '.':
    case '=':
    case ',':
    case ';':
    case '#':
    case '<':
    case '>':
    case '+':
    case '-':
    case '*':
    case '/':
    case '(':
    case ')':
      return (*g.raw);
    case ':':
      // Assign is a special case as it is a two-character token: `:=`
      if (*++g.raw != '=') {
        error("unknown token: ':%c'", *g.raw);
      }
      return TOK_ASSIGN;
    case '\0':
      return 0;
    default:
      error("unknown token: '%c'", *g.raw);
  }

  return 0;
}
