//===- LowerExpectIntrinsic.h - LowerExpectIntrinsic pass -------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
///
/// The header file for the LowerExpectIntrinsic pass as used by the new pass
/// manager.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TRANSFORMS_SCALAR_LOWEREXPECTINTRINSIC_H
#define LLVM37_TRANSFORMS_SCALAR_LOWEREXPECTINTRINSIC_H

#include "llvm37/IR/Function.h"
#include "llvm37/IR/PassManager.h"

namespace llvm37 {

class LowerExpectIntrinsicPass {
public:
  static StringRef name() { return "LowerExpectIntrinsicPass"; }

  /// \brief Run the pass over the function.
  ///
  /// This will lower all of th expect intrinsic calls in this function into
  /// branch weight metadata. That metadata will subsequently feed the analysis
  /// of the probabilities and frequencies of the CFG. After running this pass,
  /// no more expect intrinsics remain, allowing the rest of the optimizer to
  /// ignore them.
  PreservedAnalyses run(Function &F);
};

}

#endif
