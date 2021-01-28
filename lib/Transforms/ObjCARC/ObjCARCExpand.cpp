//===- ObjCARCExpand.cpp - ObjC ARC Optimization --------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file defines ObjC ARC optimizations. ARC stands for Automatic
/// Reference Counting and is a system for managing reference counts for objects
/// in Objective C.
///
/// This specific file deals with early optimizations which perform certain
/// cleanup operations.
///
/// WARNING: This file knows about certain library functions. It recognizes them
/// by name, and hardwires knowledge of their semantics.
///
/// WARNING: This file knows about how certain Objective-C library functions are
/// used. Naive LLVM37 IR transformations which would otherwise be
/// behavior-preserving may break these assumptions.
///
//===----------------------------------------------------------------------===//

#include "ObjCARC.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/InstIterator.h"
#include "llvm37/IR/Instruction.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/Value.h"
#include "llvm37/Pass.h"
#include "llvm37/PassAnalysisSupport.h"
#include "llvm37/PassRegistry.h"
#include "llvm37/PassSupport.h"
#include "llvm37/Support/Casting.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/raw_ostream.h"

#define DEBUG_TYPE "objc-arc-expand"

namespace llvm37 {
  class Module;
}

using namespace llvm37;
using namespace llvm37::objcarc;

namespace {
  /// \brief Early ARC transformations.
  class ObjCARCExpand : public FunctionPass {
    void getAnalysisUsage(AnalysisUsage &AU) const override;
    bool doInitialization(Module &M) override;
    bool runOnFunction(Function &F) override;

    /// A flag indicating whether this optimization pass should run.
    bool Run;

  public:
    static char ID;
    ObjCARCExpand() : FunctionPass(ID) {
      initializeObjCARCExpandPass(*PassRegistry::getPassRegistry());
    }
  };
}

char ObjCARCExpand::ID = 0;
INITIALIZE_PASS(ObjCARCExpand,
                "objc-arc-expand", "ObjC ARC expansion", false, false)

Pass *llvm37::createObjCARCExpandPass() {
  return new ObjCARCExpand();
}

void ObjCARCExpand::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesCFG();
}

bool ObjCARCExpand::doInitialization(Module &M) {
  Run = ModuleHasARC(M);
  return false;
}

bool ObjCARCExpand::runOnFunction(Function &F) {
  if (!EnableARCOpts)
    return false;

  // If nothing in the Module uses ARC, don't do anything.
  if (!Run)
    return false;

  bool Changed = false;

  DEBUG(dbgs() << "ObjCARCExpand: Visiting Function: " << F.getName() << "\n");

  for (inst_iterator I = inst_begin(&F), E = inst_end(&F); I != E; ++I) {
    Instruction *Inst = &*I;

    DEBUG(dbgs() << "ObjCARCExpand: Visiting: " << *Inst << "\n");

    switch (GetBasicARCInstKind(Inst)) {
    case ARCInstKind::Retain:
    case ARCInstKind::RetainRV:
    case ARCInstKind::Autorelease:
    case ARCInstKind::AutoreleaseRV:
    case ARCInstKind::FusedRetainAutorelease:
    case ARCInstKind::FusedRetainAutoreleaseRV: {
      // These calls return their argument verbatim, as a low-level
      // optimization. However, this makes high-level optimizations
      // harder. Undo any uses of this optimization that the front-end
      // emitted here. We'll redo them in the contract pass.
      Changed = true;
      Value *Value = cast<CallInst>(Inst)->getArgOperand(0);
      DEBUG(dbgs() << "ObjCARCExpand: Old = " << *Inst << "\n"
                      "               New = " << *Value << "\n");
      Inst->replaceAllUsesWith(Value);
      break;
    }
    default:
      break;
    }
  }

  DEBUG(dbgs() << "ObjCARCExpand: Finished List.\n\n");

  return Changed;
}
