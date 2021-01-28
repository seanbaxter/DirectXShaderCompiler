//===- PostDominators.cpp - Post-Dominator Calculation --------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the post-dominator construction algorithms.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Analysis/PostDominators.h"
#include "llvm37/ADT/DepthFirstIterator.h"
#include "llvm37/ADT/SetOperations.h"
#include "llvm37/IR/CFG.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/GenericDomTreeConstruction.h"
using namespace llvm37;

#define DEBUG_TYPE "postdomtree"

//===----------------------------------------------------------------------===//
//  PostDominatorTree Implementation
//===----------------------------------------------------------------------===//

char PostDominatorTree::ID = 0;
INITIALIZELLVM37_PASS(PostDominatorTree, "postdomtree",
                "Post-Dominator Tree Construction", true, true)

bool PostDominatorTree::runOnFunction(Function &F) {
  DT->recalculate(F);
  return false;
}

PostDominatorTree::~PostDominatorTree() {
  delete DT;
}

void PostDominatorTree::print(raw_ostream &OS, const Module *) const {
  DT->print(OS);
}


FunctionPass* llvm37::createPostDomTree() {
  return new PostDominatorTree();
}

