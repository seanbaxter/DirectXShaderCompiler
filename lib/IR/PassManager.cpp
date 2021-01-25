//===- PassManager.cpp - Infrastructure for managing & running IR passes --===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ADT/STLExtras.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/PassManager.h"

using namespace llvm37;

char FunctionAnalysisManagerModuleProxy::PassID;

FunctionAnalysisManagerModuleProxy::Result
FunctionAnalysisManagerModuleProxy::run(Module &M) {
  assert(FAM->empty() && "Function analyses ran prior to the module proxy!");
  return Result(*FAM);
}

FunctionAnalysisManagerModuleProxy::Result::~Result() {
  // Clear out the analysis manager if we're being destroyed -- it means we
  // didn't even see an invalidate call when we got invalidated.
  FAM->clear();
}

bool FunctionAnalysisManagerModuleProxy::Result::invalidate(
    Module &M, const PreservedAnalyses &PA) {
  // If this proxy isn't marked as preserved, then we can't even invalidate
  // individual function analyses, there may be an invalid set of Function
  // objects in the cache making it impossible to incrementally preserve them.
  // Just clear the entire manager.
  if (!PA.preserved(ID()))
    FAM->clear();

  // Return false to indicate that this result is still a valid proxy.
  return false;
}

char ModuleAnalysisManagerFunctionProxy::PassID;
