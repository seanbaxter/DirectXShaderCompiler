//===- LoopSimplifyId.cpp - ID for the Loop Canonicalization Pass ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//

#include "llvm37/Pass.h"
#include "llvm37/Analysis/AliasAnalysis.h"
#include "llvm37/Analysis/LoopInfo.h"
#include "llvm37/Analysis/ScalarEvolution.h"
#include "llvm37/Transforms/Utils/LoopSimplify.h"

using namespace llvm37;

char LoopSimplify::ID = 0;

// Publicly exposed interface to pass...
// This is in a separate file instead of LoopSimplify.cpp which brings in many dependencies
// unnecessary increasing the size of dxilconv.dll.
char &llvm37::LoopSimplifyID = LoopSimplify::ID;