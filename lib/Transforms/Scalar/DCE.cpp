//===- DCE.cpp - Code to perform dead code elimination --------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements dead inst elimination and dead code elimination.
//
// Dead Inst Elimination performs a single pass over the function removing
// instructions that are obviously dead.  Dead Code Elimination is similar, but
// it rechecks instructions that were used by removed instructions to see if
// they are newly dead.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Transforms/Scalar.h"
#include "llvm37/ADT/SetVector.h"
#include "llvm37/ADT/Statistic.h"
#include "llvm37/IR/InstIterator.h"
#include "llvm37/IR/Instruction.h"
#include "llvm37/Pass.h"
#include "llvm37/Analysis/TargetLibraryInfo.h"
#include "llvm37/Transforms/Utils/Local.h"
using namespace llvm37;

#define DEBUG_TYPE "dce"

STATISTIC(DIEEliminated, "Number of insts removed by DIE pass");
STATISTIC(DCEEliminated, "Number of insts removed");

namespace {
  //===--------------------------------------------------------------------===//
  // DeadInstElimination pass implementation
  //
  struct DeadInstElimination : public BasicBlockPass {
    static char ID; // Pass identification, replacement for typeid
    DeadInstElimination() : BasicBlockPass(ID) {
      initializeDeadInstEliminationPass(*PassRegistry::getPassRegistry());
    }
    bool runOnBasicBlock(BasicBlock &BB) override {
      if (skipOptnoneFunction(BB))
        return false;
      auto *TLIP = getAnalysisIfAvailable<TargetLibraryInfoWrapperPass>();
      TargetLibraryInfo *TLI = TLIP ? &TLIP->getTLI() : nullptr;
      bool Changed = false;
      for (BasicBlock::iterator DI = BB.begin(); DI != BB.end(); ) {
        Instruction *Inst = DI++;
        if (isInstructionTriviallyDead(Inst, TLI)) {
          Inst->eraseFromParent();
          Changed = true;
          ++DIEEliminated;
        }
      }
      return Changed;
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesCFG();
    }
  };
}

char DeadInstElimination::ID = 0;
INITIALIZELLVM37_PASS(DeadInstElimination, "die",
                "Dead Instruction Elimination", false, false)

Pass *llvm37::createDeadInstEliminationPass() {
  return new DeadInstElimination();
}


namespace {
  //===--------------------------------------------------------------------===//
  // DeadCodeElimination pass implementation
  //
  struct DCE : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    DCE() : FunctionPass(ID) {
      initializeDCEPass(*PassRegistry::getPassRegistry());
    }

    bool runOnFunction(Function &F) override;

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesCFG();
    }
 };
}

char DCE::ID = 0;
INITIALIZELLVM37_PASS(DCE, "dce", "Dead Code Elimination", false, false)

bool DCE::runOnFunction(Function &F) {
  if (skipOptnoneFunction(F))
    return false;

  auto *TLIP = getAnalysisIfAvailable<TargetLibraryInfoWrapperPass>();
  TargetLibraryInfo *TLI = TLIP ? &TLIP->getTLI() : nullptr;

  // Start out with all of the instructions in the worklist...
  SmallSetVector<Instruction*, 16> WorkList;
  for (inst_iterator i = inst_begin(F), e = inst_end(F); i != e; ++i)
    WorkList.insert(&*i);

  // Loop over the worklist finding instructions that are dead.  If they are
  // dead make them drop all of their uses, making other instructions
  // potentially dead, and work until the worklist is empty.
  //
  bool MadeChange = false;
  while (!WorkList.empty()) {
    Instruction *I = WorkList.pop_back_val();

    if (isInstructionTriviallyDead(I, TLI)) { // If the instruction is dead.
      // Loop over all of the values that the instruction uses, if there are
      // instructions being used, add them to the worklist, because they might
      // go dead after this one is removed.
      //
      for (User::op_iterator OI = I->op_begin(), E = I->op_end(); OI != E; ++OI)
        if (Instruction *Used = dyn_cast<Instruction>(*OI))
          WorkList.insert(Used);

      // Remove the instruction.
      I->eraseFromParent();

      MadeChange = true;
      ++DCEEliminated;
    }
  }
  return MadeChange;
}

FunctionPass *llvm37::createDeadCodeEliminationPass() {
  return new DCE();
}

