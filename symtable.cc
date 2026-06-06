#include "symtable.h"

#include "lexer.h"
#include "global.h"
#include "util.h"

#include <cstdlib>
#include <cstring>

SymTableEntry* head = nullptr;


void init_sym_table() {
  SymTableEntry* new_entry = new SymTableEntry;
  
  new_entry->depth = 0;
  new_entry->type = TOK_PROCEDURE;
  new_entry->name = strdup("main");
  new_entry->next = nullptr;

  head = new_entry;
}

void add_symbol(int symbol_type) {
  SymTableEntry* curr = head;

  for (;;) {
    if (!strcmp(curr->name, g.token)) {
      if (curr->depth == (g.depth - 1)) {
        error("duplicated symbol: %s", g.token);
      }
    }

    if (curr->next == nullptr) {
      break;
    }

    curr = curr->next;
  }

  SymTableEntry* new_entry = new SymTableEntry;

  new_entry->depth = g.depth - 1;
  new_entry->type = symbol_type;
  new_entry->name = strdup(g.token);
  new_entry->next = nullptr;

  curr->next = new_entry;
}

// Destroy constants and variables for the procedure we just left
void destroy_symbols() {
  SymTableEntry* curr = head;
  SymTableEntry* prev;

again:
  curr = head;

  // Walk the link list until we hit the last element
  while(curr->next != nullptr) {
    prev = curr;
    curr = curr->next;
  }

  // Delete the symbol unless it is a procedure
  if (curr->type != TOK_PROCEDURE) {
    free(curr->name);
    free(curr);
    prev->next = nullptr;
    goto again;
  }
}
