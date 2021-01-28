//==- CFGReachabilityAnalysis.h - Basic reachability analysis ----*- C++ -*-==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines a flow-sensitive, (mostly) path-insensitive reachability
// analysis based on Clang's CFGs.  Clients can query if a given basic block
// is reachable within the CFG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_ANALYSIS_ANALYSES_CFGREACHABILITYANALYSIS_H
#define LLVM37_CLANG_ANALYSIS_ANALYSES_CFGREACHABILITYANALYSIS_H

#include "llvm37/ADT/BitVector.h"
#include "llvm37/ADT/DenseMap.h"

namespace clang {

class CFG;
class CFGBlock;
  
// A class that performs reachability queries for CFGBlocks. Several internal
// checks in this checker require reachability information. The requests all
// tend to have a common destination, so we lazily do a predecessor search
// from the destination node and cache the results to prevent work
// duplication.
class CFGReverseBlockReachabilityAnalysis {
  typedef llvm37::BitVector ReachableSet;
  typedef llvm37::DenseMap<unsigned, ReachableSet> ReachableMap;
  ReachableSet analyzed;
  ReachableMap reachable;
public:
  CFGReverseBlockReachabilityAnalysis(const CFG &cfg);

  /// Returns true if the block 'Dst' can be reached from block 'Src'.
  bool isReachable(const CFGBlock *Src, const CFGBlock *Dst);

private:
  void mapReachability(const CFGBlock *Dst);
};
  
}

#endif
