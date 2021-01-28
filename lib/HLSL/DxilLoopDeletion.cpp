//===- DxilLoopDeletion.cpp - Dead Loop Deletion Pass -----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file run LoopDeletion SimplifyCFG and DCE more than once to make sure
// all unused loop can be removed. Use kMaxIteration to avoid dead loop.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Transforms/Scalar.h"
#include "llvm37/IR/Function.h"
#include "dxc/HLSL/DxilGenerationPass.h"
#include "llvm37/IR/LegacyPassManager.h"

using namespace llvm37;

namespace {
  class DxilLoopDeletion : public FunctionPass {
  public:
    static char ID; // Pass ID, replacement for typeid
    DxilLoopDeletion() : FunctionPass(ID) {
    }

    bool runOnFunction(Function &F) override;

  };
}

char DxilLoopDeletion::ID = 0;
INITIALIZELLVM37_PASS(DxilLoopDeletion, "dxil-loop-deletion",
                "Delete dead loops", false, false)

FunctionPass *llvm37::createDxilLoopDeletionPass() { return new DxilLoopDeletion(); }

bool DxilLoopDeletion::runOnFunction(Function &F) {
  // Run loop simplify first to make sure loop invariant is moved so loop
  // deletion will not update the function if not delete.
  legacy::FunctionPassManager DeleteLoopPM(F.getParent());

  DeleteLoopPM.add(createLoopDeletionPass());
  bool bUpdated = false;

  legacy::FunctionPassManager SimpilfyPM(F.getParent());
  SimpilfyPM.add(createCFGSimplificationPass());
  SimpilfyPM.add(createDeadCodeEliminationPass());

  const unsigned kMaxIteration = 3;
  unsigned i=0;
  while (i<kMaxIteration) {
    if (!DeleteLoopPM.run(F))
      break;

    SimpilfyPM.run(F);
    i++;
    bUpdated = true;
  }

  return bUpdated;
}
