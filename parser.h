#pragma once

#define CHECK_LSH 0
#define CHECK_RHS 1
#define CHECK_CALL 2

void next();
void expect(int match);
void block();
void statement();
void expression();
void condition();
void facor();
void term();
void symcheck(int check);

// Parse the source program pointed to by `g.raw`, printing each token.
void parse();
