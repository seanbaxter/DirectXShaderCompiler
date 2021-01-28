//=- Dominators.cpp - Implementation of dominators tree for Clang CFG C++ -*-=//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Analysis/Analyses/Dominators.h"

using namespace clang;

void DominatorTree::anchor() { }
