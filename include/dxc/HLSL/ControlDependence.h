///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// ComputeViewIdSets.h                                                       //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Computes control dependence relation for a function.                      //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "llvm37/IR/BasicBlock.h"
#include "llvm37/IR/Dominators.h"

#include <unordered_set>
#include <unordered_map>

namespace llvm37 {
  class Function;
  class raw_ostream;
}


namespace hlsl {

using BasicBlockSet = std::unordered_set<llvm37::BasicBlock *>;
using PostDomRelationType = llvm37::DominatorTreeBase<llvm37::BasicBlock>;

class ControlDependence {
public:
  void Compute(llvm37::Function *F, PostDomRelationType &PostDomRel);
  void Clear();
  const BasicBlockSet &GetCDBlocks(llvm37::BasicBlock *pBB) const;
  void print(llvm37::raw_ostream &OS);
  void dump();

private:
  using BasicBlockVector = std::vector<llvm37::BasicBlock *>;
  using ControlDependenceType = std::unordered_map<llvm37::BasicBlock *, BasicBlockSet>;

  llvm37::Function *m_pFunc;
  ControlDependenceType m_ControlDependence;
  BasicBlockSet m_EmptyBBSet;

  llvm37::BasicBlock *GetIPostDom(PostDomRelationType &PostDomRel, llvm37::BasicBlock *pBB);
  void ComputeRevTopOrderRec(PostDomRelationType &PostDomRel, llvm37::BasicBlock *pBB,
                             BasicBlockVector &RevTopOrder, BasicBlockSet &VisitedBBs);
};

} // end of hlsl namespace
