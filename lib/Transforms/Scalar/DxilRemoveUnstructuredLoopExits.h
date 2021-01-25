//===- DxilRemoveUnstructuredLoopExits.h - Make unrolled loops structured ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <unordered_set>

namespace llvm37 {
  class Loop;
  class LoopInfo;
  class DominatorTree;
  class BasicBlock;
}

namespace hlsl {

  // exclude_set is a list of *EXIT BLOCKS* to exclude (NOTE: not *exiting* blocks)
  bool RemoveUnstructuredLoopExits(llvm37::Loop *L, llvm37::LoopInfo *LI, llvm37::DominatorTree *DT, std::unordered_set<llvm37::BasicBlock *> *exclude_set = nullptr);
}

