//===- BasicTargetTransformInfo.cpp - Basic target-independent TTI impl ---===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
/// \file
/// This file provides the implementation of a basic TargetTransformInfo pass
/// predicated on the target abstractions present in the target independent
/// code generator. It uses these (primarily TargetLowering) to model as much
/// of the TTI query interface as possible. It is included by most targets so
/// that they can specialize only a small subset of the query space.
///
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/BasicTTIImpl.h"
#include "llvm37/Analysis/LoopInfo.h"
#include "llvm37/Analysis/TargetTransformInfo.h"
#include "llvm37/Analysis/TargetTransformInfoImpl.h"
#include "llvm37/CodeGen/Passes.h"
#include "llvm37/Support/CommandLine.h"
#include <utility>
using namespace llvm37;

#define DEBUG_TYPE "basictti"

// This flag is used by the template base class for BasicTTIImpl, and here to
// provide a definition.
cl::opt<unsigned>
    llvm37::PartialUnrollingThreshold("partial-unrolling-threshold", cl::init(0),
                                    cl::desc("Threshold for partial unrolling"),
                                    cl::Hidden);

BasicTTIImpl::BasicTTIImpl(const TargetMachine *TM, Function &F)
    : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
      TLI(ST->getTargetLowering()) {}
