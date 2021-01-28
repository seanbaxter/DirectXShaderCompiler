//===-- ShadowStackGC.cpp - GC support for uncooperative targets ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements lowering for the llvm37.gc* intrinsics for targets that do
// not natively support them (which includes the C backend). Note that the code
// generated is not quite as efficient as algorithms which generate stack maps
// to identify roots.
//
// This pass implements the code transformation described in this paper:
//   "Accurate Garbage Collection in an Uncooperative Environment"
//   Fergus Henderson, ISMM, 2002
//
// In runtime/GC/SemiSpace.cpp is a prototype runtime which is compatible with
// ShadowStackGC.
//
// In order to support this particular transformation, all stack roots are
// coallocated in the stack. This allows a fully target-independent stack map
// while introducing only minor runtime overhead.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/GCs.h"
#include "llvm37/ADT/StringExtras.h"
#include "llvm37/CodeGen/GCStrategy.h"
#include "llvm37/IR/CallSite.h"
#include "llvm37/IR/IRBuilder.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/Module.h"

using namespace llvm37;

#define DEBUG_TYPE "shadowstackgc"

namespace {
class ShadowStackGC : public GCStrategy {
public:
  ShadowStackGC();
};
}

static GCRegistry::Add<ShadowStackGC>
    X("shadow-stack", "Very portable GC for uncooperative code generators");

void llvm37::linkShadowStackGC() {}

ShadowStackGC::ShadowStackGC() {
  InitRoots = true;
  CustomRoots = true;
}
