//===-- IR/Statepoint.cpp -- gc.statepoint utilities ---  -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// 
//===----------------------------------------------------------------------===//

#include "llvm37/IR/Function.h"
#include "llvm37/IR/Constant.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/Statepoint.h"
#include "llvm37/Support/CommandLine.h"

using namespace std;
using namespace llvm37;

bool llvm37::isStatepoint(const ImmutableCallSite &CS) {
  if (!CS.getInstruction()) {
    // This is not a call site
    return false;
  }

  const Function *F = CS.getCalledFunction();
  return (F && F->getIntrinsicID() == Intrinsic::experimental_gc_statepoint);
}
bool llvm37::isStatepoint(const Value *inst) {
  if (isa<InvokeInst>(inst) || isa<CallInst>(inst)) {
    ImmutableCallSite CS(inst);
    return isStatepoint(CS);
  }
  return false;
}
bool llvm37::isStatepoint(const Value &inst) {
  return isStatepoint(&inst);
}

bool llvm37::isGCRelocate(const ImmutableCallSite &CS) {
  if (!CS.getInstruction()) {
    // This is not a call site
    return false;
  }

  return isGCRelocate(CS.getInstruction());
}
bool llvm37::isGCRelocate(const Value *inst) {
  if (const CallInst *call = dyn_cast<CallInst>(inst)) {
    if (const Function *F = call->getCalledFunction()) {
      return F->getIntrinsicID() == Intrinsic::experimental_gc_relocate;
    }
  }
  return false;
}

bool llvm37::isGCResult(const ImmutableCallSite &CS) {
  if (!CS.getInstruction()) {
    // This is not a call site
    return false;
  }

  return isGCResult(CS.getInstruction());
}
bool llvm37::isGCResult(const Value *inst) {
  if (const CallInst *call = dyn_cast<CallInst>(inst)) {
    if (Function *F = call->getCalledFunction()) {
      return (F->getIntrinsicID() == Intrinsic::experimental_gc_result_int ||
              F->getIntrinsicID() == Intrinsic::experimental_gc_result_float ||
              F->getIntrinsicID() == Intrinsic::experimental_gc_result_ptr ||
              F->getIntrinsicID() == Intrinsic::experimental_gc_result);
    }
  }
  return false;
}
