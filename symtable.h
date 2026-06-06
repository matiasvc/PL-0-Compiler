
struct SymTableEntry {
  int depth;
  int type;
  char* name;
  struct SymTableEntry* next;
};

void init_sym_table();
void add_symbol(int type);
void destroy_symbols();

extern SymTableEntry* head;
