#pragma once

namespace llvm37
{
  class Function;
}

// Analyzes the reducibility of the control flow graph of F and uses node splitting
// to make an irredicible CFG reducible. Returns the number of node splits.
int makeReducible(llvm37::Function* F);