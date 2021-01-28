//===- ReachableCode.h -----------------------------------------*- C++ --*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// A flow-sensitive, path-insensitive analysis of unreachable code.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_ANALYSIS_ANALYSES_REACHABLECODE_H
#define LLVM37_CLANG_ANALYSIS_ANALYSES_REACHABLECODE_H

#include "clang/Basic/SourceLocation.h"

//===----------------------------------------------------------------------===//
// Forward declarations.
//===----------------------------------------------------------------------===//

namespace llvm37 {
  class BitVector;
}

namespace clang {
  class AnalysisDeclContext;
  class CFGBlock;
  class Preprocessor;
}

//===----------------------------------------------------------------------===//
// API.
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

namespace clang {
namespace reachable_code {

/// Classifications of unreachable code.
enum UnreachableKind {
  UK_Return,
  UK_Break,
  UK_Loop_Increment,
  UK_Other
};

class Callback {
  virtual void anchor();
public:
  virtual ~Callback() {}
  virtual void HandleUnreachable(UnreachableKind UK,
                                 SourceLocation L,
                                 SourceRange ConditionVal,
                                 SourceRange R1,
                                 SourceRange R2) = 0;
};

/// ScanReachableFromBlock - Mark all blocks reachable from Start.
/// Returns the total number of blocks that were marked reachable.  
unsigned ScanReachableFromBlock(const CFGBlock *Start,
                                llvm37::BitVector &Reachable);

void FindUnreachableCode(AnalysisDeclContext &AC, Preprocessor &PP,
                         Callback &CB);

}} // end namespace clang::reachable_code

#endif
