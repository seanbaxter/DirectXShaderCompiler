//===-- ObjCARC.cpp -------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements common infrastructure for libLLVM37ObjCARCOpts.a, which
// implements several scalar transformations over the LLVM37 intermediate
// representation, including the C bindings for that library.
//
//===----------------------------------------------------------------------===//

#include "ObjCARC.h"
#include "llvm37-c/Core.h"
#include "llvm37-c/Initialization.h"
#include "llvm37/InitializePasses.h"
#include "llvm37/Support/CommandLine.h"

namespace llvm37 {
  class PassRegistry;
}

using namespace llvm37;
using namespace llvm37::objcarc;

/// \brief A handy option to enable/disable all ARC Optimizations.
bool llvm37::objcarc::EnableARCOpts;
static cl::opt<bool, true>
EnableARCOptimizations("enable-objc-arc-opts",
                       cl::desc("enable/disable all ARC Optimizations"),
                       cl::location(EnableARCOpts),
                       cl::init(true));

/// initializeObjCARCOptsPasses - Initialize all passes linked into the
/// ObjCARCOpts library.
void llvm37::initializeObjCARCOpts(PassRegistry &Registry) {
  initializeObjCARCAliasAnalysisPass(Registry);
  initializeObjCARCAPElimPass(Registry);
  initializeObjCARCExpandPass(Registry);
  initializeObjCARCContractPass(Registry);
  initializeObjCARCOptPass(Registry);
  initializePAEvalPass(Registry);
}

void LLVM37InitializeObjCARCOpts(LLVM37PassRegistryRef R) {
  initializeObjCARCOpts(*unwrap(R));
}
