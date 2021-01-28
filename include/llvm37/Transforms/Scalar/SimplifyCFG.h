//===- SimplifyCFG.h - Simplify and canonicalize the CFG --------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file provides the interface for the pass responsible for both
/// simplifying and canonicalizing the CFG.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TRANSFORMS_SCALAR_SIMPLIFYCFG_H
#define LLVM37_TRANSFORMS_SCALAR_SIMPLIFYCFG_H

#include "llvm37/IR/Function.h"
#include "llvm37/IR/PassManager.h"

namespace llvm37 {

/// \brief A pass to simplify and canonicalize the CFG of a function.
///
/// This pass iteratively simplifies the entire CFG of a function, removing
/// unnecessary control flows and bringing it into the canonical form expected
/// by the rest of the mid-level optimizer.
class SimplifyCFGPass {
  int BonusInstThreshold;

public:
  static StringRef name() { return "SimplifyCFGPass"; }

  /// \brief Construct a pass with the default thresholds.
  SimplifyCFGPass();

  /// \brief Construct a pass with a specific bonus threshold.
  SimplifyCFGPass(int BonusInstThreshold);

  /// \brief Run the pass over the function.
  PreservedAnalyses run(Function &F, AnalysisManager<Function> *AM);
};

}

#endif
