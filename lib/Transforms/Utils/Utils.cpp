//===-- Utils.cpp - TransformUtils Infrastructure -------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the common initialization infrastructure for the
// TransformUtils library.
//
//===----------------------------------------------------------------------===//

#include "llvm37/InitializePasses.h"
#include "llvm37-c/Initialization.h"
#include "llvm37/PassRegistry.h"

using namespace llvm37;

/// initializeTransformUtils - Initialize all passes in the TransformUtils
/// library.
void llvm37::initializeTransformUtils(PassRegistry &Registry) {
  initializeAddDiscriminatorsPass(Registry);
  initializeBreakCriticalEdgesPass(Registry);
  initializeInstNamerPass(Registry);
  initializeLCSSAPass(Registry);
  initializeLoopSimplifyPass(Registry);
  initializeLowerInvokePass(Registry);
  initializeLowerSwitchPass(Registry);
  initializePromotePassPass(Registry);
  initializeUnifyFunctionExitNodesPass(Registry);
  initializeInstSimplifierPass(Registry);
  initializeMetaRenamerPass(Registry);
}

/// LLVM37InitializeTransformUtils - C binding for initializeTransformUtilsPasses.
void LLVM37InitializeTransformUtils(LLVM37PassRegistryRef R) {
  initializeTransformUtils(*unwrap(R));
}
