//===- Reg2Mem.cpp - Convert registers to allocas -------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file demotes all registers to memory references.  It is intended to be
// the inverse of PromoteMemoryToRegister.  By converting to loads, the only
// values live across basic blocks are allocas and loads before phi nodes.
// It is intended that this should make CFG hacking much easier.
// To make later hacking easier, the entry block is split into two, such that
// all introduced allocas and nothing else are in the entry block.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Transforms/Scalar.h"
#include "llvm37/ADT/Statistic.h"
#include "llvm37/IR/BasicBlock.h"
#include "llvm37/IR/CFG.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Pass.h"
#include "llvm37/Transforms/Utils/Local.h"
#include <list>
using namespace llvm37;

#define DEBUG_TYPE "reg2mem"

STATISTIC(NumRegsDemoted, "Number of registers demoted");
STATISTIC(NumPhisDemoted, "Number of phi-nodes demoted");

namespace {
  struct RegToMem : public FunctionPass {
    static char ID; // Pass identification, replacement for typeid
    RegToMem() : FunctionPass(ID) {
      initializeRegToMemPass(*PassRegistry::getPassRegistry());
    }

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.addRequiredID(BreakCriticalEdgesID);
      AU.addPreservedID(BreakCriticalEdgesID);
    }

    bool valueEscapes(const Instruction *Inst) const {
      const BasicBlock *BB = Inst->getParent();
      for (const User *U : Inst->users()) {
        const Instruction *UI = cast<Instruction>(U);
        if (UI->getParent() != BB || isa<PHINode>(UI))
          return true;
      }
      return false;
    }

    bool runOnFunction(Function &F) override;
  };
}

char RegToMem::ID = 0;
INITIALIZELLVM37_PASS_BEGIN(RegToMem, "reg2mem", "Demote all values to stack slots",
                false, false)
INITIALIZELLVM37_PASS_DEPENDENCY(BreakCriticalEdges)
INITIALIZELLVM37_PASS_END(RegToMem, "reg2mem", "Demote all values to stack slots",
                false, false)

bool RegToMem::runOnFunction(Function &F) {
  if (F.isDeclaration())
    return false;

  // Insert all new allocas into entry block.
  BasicBlock *BBEntry = &F.getEntryBlock();
  assert(pred_empty(BBEntry) &&
         "Entry block to function must not have predecessors!");

  // Find first non-alloca instruction and create insertion point. This is
  // safe if block is well-formed: it always have terminator, otherwise
  // we'll get and assertion.
  BasicBlock::iterator I = BBEntry->begin();
  while (isa<AllocaInst>(I)) ++I;

  CastInst *AllocaInsertionPoint =
    new BitCastInst(Constant::getNullValue(Type::getInt32Ty(F.getContext())),
                    Type::getInt32Ty(F.getContext()),
                    "reg2mem alloca point", I);

  // Find the escaped instructions. But don't create stack slots for
  // allocas in entry block.
  std::list<Instruction*> WorkList;
  for (Function::iterator ibb = F.begin(), ibe = F.end();
       ibb != ibe; ++ibb)
    for (BasicBlock::iterator iib = ibb->begin(), iie = ibb->end();
         iib != iie; ++iib) {
      if (!(isa<AllocaInst>(iib) && iib->getParent() == BBEntry) &&
          valueEscapes(iib)) {
        WorkList.push_front(&*iib);
      }
    }

  // Demote escaped instructions
  NumRegsDemoted += WorkList.size();
  for (std::list<Instruction*>::iterator ilb = WorkList.begin(),
       ile = WorkList.end(); ilb != ile; ++ilb)
    DemoteRegToStack(**ilb, false, AllocaInsertionPoint);

  WorkList.clear();

  // Find all phi's
  for (Function::iterator ibb = F.begin(), ibe = F.end();
       ibb != ibe; ++ibb)
    for (BasicBlock::iterator iib = ibb->begin(), iie = ibb->end();
         iib != iie; ++iib)
      if (isa<PHINode>(iib))
        WorkList.push_front(&*iib);

  // Demote phi nodes
  NumPhisDemoted += WorkList.size();
  for (std::list<Instruction*>::iterator ilb = WorkList.begin(),
       ile = WorkList.end(); ilb != ile; ++ilb)
    DemotePHIToStack(cast<PHINode>(*ilb), AllocaInsertionPoint);

  return true;
}


// createDemoteRegisterToMemory - Provide an entry point to create this pass.
char &llvm37::DemoteRegisterToMemoryID = RegToMem::ID;
FunctionPass *llvm37::createDemoteRegisterToMemoryPass() {
  return new RegToMem();
}
