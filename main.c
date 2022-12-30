#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>

#include <bsd/stdlib.h>

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

#define TOK_IDENT	'I'
#define TOK_NUMBER	'N'
#define TOK_CONST	'C'
#define TOK_VAR		'V'
#define TOK_PROCEDURE	'P'
#define TOK_CALL	'c'
#define TOK_BEGIN	'B'
#define TOK_END		'E'
#define TOK_IF		'i'
#define TOK_THEN	'T'
#define TOK_WHILE	'W'
#define TOK_DO		'D'
#define TOK_ODD		'O'
#define TOK_DOT		'.'
#define TOK_EQUAL	'='
#define TOK_COMMA	','
#define TOK_SEMICOLON	';'
#define TOK_ASSIGN	':'
#define TOK_HASH	'#'
#define TOK_LESSTHAN	'<'
#define TOK_GREATERTHAN	'>'
#define TOK_PLUS	'+'
#define TOK_MINUS	'-'
#define TOK_MULTIPLY	'*'
#define TOK_DIVIDE	'/'
#define TOK_LPAREN	'('
#define TOK_RPAREN	')'

static char* raw;
static char* token;
static int type;
static size_t line = 1;

/*** Function declerations ***/

// Misc functions
static void error(const char* fmt, ...);
static void readin(char* file);

/*** Lexer ***/

static void comment(void) {
  int ch;

  while ((ch = *raw++) != '}') {
    if (ch == '\0') {
      error("unterminated comment");
    }

    if (ch == '\n') {
      ++line;
    }
  }
}

// Identify the token pointet to by `raw`
static int ident(void) {
  char* p;
  size_t i;
  size_t len;

  // Save the current position of `raw` in `p`
  p = raw;

  // Loop through `raw` until we hit a character that is not alphanumeric or `_`
  while (isalnum(*raw) || *raw == '_') {
    ++raw;
  }

  // Get the size of our token
  len = raw - p;

  // Revert one step as the `le
  --raw;

  // Allocate memory to copy the token string into
  free(token);
  if ((token = malloc(len + 1)) == NULL) {
    error("malloc failed");
  }

  // Copy token string into `token` and append a null-character
  for (i = 0; i < len; i++) {
    token[i] = *p++;
  }
  token[i] = '\0';

  // Detect the type of token
  if (!strcmp(token, "const")) {
    return TOK_CONST;
  } else if(!strcmp(token, "var")) {
    return TOK_VAR;
  } else if(!strcmp(token, "procedure")) {
    return TOK_PROCEDURE;
  } else if(!strcmp(token, "call")) {
    return TOK_CALL;
  } else if(!strcmp(token, "begin")) {
    return TOK_BEGIN;
  } else if(!strcmp(token, "end")) {
    return TOK_END;
  } else if(!strcmp(token, "if")) {
    return TOK_IF;
  } else if(!strcmp(token, "then")) {
    return TOK_THEN;
  } else if(!strcmp(token, "while")) {
    return TOK_WHILE;
  } else if(!strcmp(token, "do")) {
    return TOK_DO;
  } else if(!strcmp(token, "odd")) {
    return TOK_ODD;
  } else {
    // String did not match any keyword, so it must be an identifier
    return TOK_IDENT;
  }
}

// Parse the number pointed to by `raw`
static int number(void) {
  const char* errstr = NULL;
  char* p;
  size_t i;
  size_t j = 0;
  size_t len;

  p = raw;
  while (isdigit(*raw) || *raw == '_') {
    ++raw;
  }

  len = raw - p;
  --raw;

  free(token);
  if((token = malloc(len+1)) == NULL) {
    error("malloc failed");
  }

  for (i = 0; i < len; ++i) {
    if (isdigit(*p)) {
      token[j++] = *(p++);
    }
    token[j] = '\0';
  }

  (void) strtonum(token, 0, LONG_MAX, &errstr);
  if (errstr != NULL) {
    error("invalid number: %s", token);
  }

  return TOK_NUMBER;
}

// Lex the token pointed to by `raw`
static int lex(void) {
again:
  // Skip any whitespace
  while(*raw == ' ' || *raw == '\t' || *raw == '\n') {
    if (*raw == '\n') {
      ++line;
    }
    ++raw;
  }

  if (isalpha(*raw) || *raw == '_') {
    return ident();
  }

  if (isdigit(*raw)) {
    return number();
  }

  switch (*raw) {
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
      return (*raw);
    case ':':
      // Assign is a special case as it is a two-character token: `:=`
      if (*++raw != '=') {
        error("unknown token: ':%c'", *raw);
      }
      return TOK_ASSIGN;
    case '\0':
      return 0;
    default:
      error("unknown token: '%c'", *raw);
  }

  return 0;
}

/*** Parser ***/

static void parse(void) {
  while ((type = lex()) != 0) {
    ++raw;
    (void) fprintf(stdout, "%lu|%d\t", line, type);

    switch (type) {
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
        (void) fprintf(stdout, "%s (%c)", token, type);
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
        (void) fputc(type, stdout);
        break;
      case TOK_ASSIGN:
        (void) fputs(":=", stdout);
    }
    (void) fputc('\n', stdout);
  }
}

/*** Misc functions ***/

static void error(const char* fmt, ...) {
  va_list ap;

  (void) fprintf(stderr, "pl0c: error: %lu ", line);

  va_start(ap, fmt);
  (void) vfprintf(stderr, fmt, ap);
  va_end(ap);

  (void) fputc('\n', stderr);

  exit(1);
}

static void readin(char* file) {
  int fd;
  struct stat st;

  if (strrchr(file, '.') == NULL) {
    error("file must end in '.pl0'");
  }

  if (strcmp(strrchr(file, '.'), ".pl0") != 0) {
    error("file must end in '.pl0'");
  }

  if ((fd = open(file, O_RDONLY)) == -1) {
    error("couldn't open %s", file);
  }

  if (fstat(fd, &st) == -1) {
    error("coudn't get file size");
  }

  // Allocate space for entire file pluss null-character
  if ((raw = malloc(st.st_size + 1)) == NULL) {
    error("malloc failed");
  }

  if (read(fd, raw, st.st_size) != st.st_size) {
    error("couldn't read %s", file);
  }

  // Terminate the string
  raw[st.st_size] = '\0';

  (void) close(fd);
}

/*** Main ***/

int main(int argc, char** argv) {

  char* startp;

  if (argc != 2) {
    (void) fputs("usage: pl0c file.pl0\n", stderr);
    exit(1);
  }

  readin(argv[1]);
  startp = raw;

  parse();

  free(startp);

  return 0;
}
