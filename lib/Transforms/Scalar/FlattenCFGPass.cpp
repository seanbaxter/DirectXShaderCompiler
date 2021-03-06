//===- FlattenCFGPass.cpp - CFG Flatten Pass ----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements flattening of CFG.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Transforms/Scalar.h"
#include "llvm37/Analysis/AliasAnalysis.h"
#include "llvm37/IR/CFG.h"
#include "llvm37/Pass.h"
#include "llvm37/Transforms/Utils/Local.h"
using namespace llvm37;

#define DEBUG_TYPE "flattencfg"

namespace {
struct FlattenCFGPass : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
public:
  FlattenCFGPass() : FunctionPass(ID) {
    initializeFlattenCFGPassPass(*PassRegistry::getPassRegistry());
  }
  bool runOnFunction(Function &F) override;

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.addRequired<AliasAnalysis>();
  }

private:
  AliasAnalysis *AA;
};
}

char FlattenCFGPass::ID = 0;
INITIALIZELLVM37_PASS_BEGIN(FlattenCFGPass, "flattencfg", "Flatten the CFG", false,
                      false)
INITIALIZELLVM37_AG_DEPENDENCY(AliasAnalysis)
INITIALIZELLVM37_PASS_END(FlattenCFGPass, "flattencfg", "Flatten the CFG", false,
                    false)

// Public interface to the FlattenCFG pass
FunctionPass *llvm37::createFlattenCFGPass() { return new FlattenCFGPass(); }

/// iterativelyFlattenCFG - Call FlattenCFG on all the blocks in the function,
/// iterating until no more changes are made.
static bool iterativelyFlattenCFG(Function &F, AliasAnalysis *AA) {
  bool Changed = false;
  bool LocalChange = true;
  while (LocalChange) {
    LocalChange = false;

    // Loop over all of the basic blocks and remove them if they are unneeded...
    //
    for (Function::iterator BBIt = F.begin(); BBIt != F.end();) {
      if (FlattenCFG(BBIt++, AA)) {
        LocalChange = true;
      }
    }
    Changed |= LocalChange;
  }
  return Changed;
}

bool FlattenCFGPass::runOnFunction(Function &F) {
  AA = &getAnalysis<AliasAnalysis>();
  bool EverChanged = false;
  // iterativelyFlattenCFG can make some blocks dead.
  while (iterativelyFlattenCFG(F, AA)) {
    removeUnreachableBlocks(F);
    EverChanged = true;
  }
  return EverChanged;
}
