//===- LoopSimplify.h - Loop Canonicalization Pass ----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// See LoopSimplify.cpp for description of the pass.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Transforms/Scalar.h"
using namespace llvm37;

struct LoopSimplify : public FunctionPass {
  static char ID; // Pass identification, replacement for typeid
  LoopSimplify();

  // AA - If we have an alias analysis object to update, this is it, otherwise
  // this is null.
  AliasAnalysis* AA;
  DominatorTree* DT;
  LoopInfo* LI;
  ScalarEvolution* SE;
  AssumptionCache* AC;

  bool runOnFunction(llvm37::Function& F) override;
  void getAnalysisUsage(llvm37::AnalysisUsage& AU) const override;

  /// verifyAnalysis() - Verify LoopSimplifyForm's guarantees.
  void verifyAnalysis() const override;
};
