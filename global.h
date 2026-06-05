#pragma once

#include <cstddef>

// Global compiler state.
struct Globals {
  char* raw = nullptr;    // Current position in the source buffer.
  char* token = nullptr;  // Text of the most recent identifier or number token.
  int type = 0;           // Type of the most recent token.
  int depth = 0;          // Current recursion depth
  size_t line = 1;        // Current line number, starting at 1.
};

// The single instance, defined in global.cc.
extern Globals g;
