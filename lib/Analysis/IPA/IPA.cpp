//===-- IPA.cpp -----------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the common initialization routines for the IPA library.
//
//===----------------------------------------------------------------------===//

#include "llvm37/InitializePasses.h"
#include "llvm37-c/Initialization.h"
#include "llvm37/PassRegistry.h"

using namespace llvm37;

/// initializeIPA - Initialize all passes linked into the IPA library.
void llvm37::initializeIPA(PassRegistry &Registry) {
  initializeCallGraphWrapperPassPass(Registry);
  initializeCallGraphPrinterPass(Registry);
  initializeCallGraphViewerPass(Registry);
  initializeGlobalsModRefPass(Registry);
}

void LLVM37InitializeIPA(LLVM37PassRegistryRef R) {
  initializeIPA(*unwrap(R));
}
