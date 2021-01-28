//===-- Instrumentation.cpp - TransformUtils Infrastructure ---------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the common initialization infrastructure for the
// Instrumentation library.
//
//===----------------------------------------------------------------------===//

#include "llvm37/InitializePasses.h"
#include "llvm37-c/Initialization.h"
#include "llvm37/PassRegistry.h"

using namespace llvm37;

/// initializeInstrumentation - Initialize all passes in the TransformUtils
/// library.
void llvm37::initializeInstrumentation(PassRegistry &Registry) {
  initializeAddressSanitizerPass(Registry);
  initializeAddressSanitizerModulePass(Registry);
  initializeBoundsCheckingPass(Registry);
  initializeGCOVProfilerPass(Registry);
  initializeInstrProfilingPass(Registry);
  initializeMemorySanitizerPass(Registry);
  initializeThreadSanitizerPass(Registry);
  initializeSanitizerCoverageModulePass(Registry);
  initializeDataFlowSanitizerPass(Registry);
  initializeSafeStackPass(Registry);
}

/// LLVM37InitializeInstrumentation - C binding for
/// initializeInstrumentation.
void LLVM37InitializeInstrumentation(LLVM37PassRegistryRef R) {
  initializeInstrumentation(*unwrap(R));
}
