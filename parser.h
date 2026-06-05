#pragma once

void next();
void expect(int match);
void block();
void statement();
void expression();
void condition();
void facor();
void term();

// Parse the source program pointed to by `g.raw`, printing each token.
void parse();
