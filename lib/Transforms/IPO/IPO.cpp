//===-- IPO.cpp -----------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the common infrastructure (including C bindings) for 
// libLLVM37IPO.a, which implements several transformations over the LLVM37 
// intermediate representation.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/Initialization.h"
#include "llvm37-c/Transforms/IPO.h"
#include "llvm37/InitializePasses.h"
#include "llvm37/IR/LegacyPassManager.h"
#include "llvm37/Transforms/IPO.h"

using namespace llvm37;

void llvm37::initializeIPO(PassRegistry &Registry) {
  initializeArgPromotionPass(Registry);
  initializeConstantMergePass(Registry);
  initializeDAEPass(Registry);
  initializeDAHPass(Registry);
  initializeFunctionAttrsPass(Registry);
  initializeGlobalDCEPass(Registry);
  initializeGlobalOptPass(Registry);
  initializeIPCPPass(Registry);
  initializeAlwaysInlinerPass(Registry);
  initializeSimpleInlinerPass(Registry);
  initializeInternalizePassPass(Registry);
  initializeLoopExtractorPass(Registry);
  initializeBlockExtractorPassPass(Registry);
  initializeSingleLoopExtractorPass(Registry);
  initializeLowerBitSetsPass(Registry);
  initializeMergeFunctionsPass(Registry);
  initializePartialInlinerPass(Registry);
  initializePruneEHPass(Registry);
  initializeStripDeadPrototypesPassPass(Registry);
  initializeStripSymbolsPass(Registry);
  initializeStripDebugDeclarePass(Registry);
  initializeStripDeadDebugInfoPass(Registry);
  initializeStripNonDebugSymbolsPass(Registry);
  initializeBarrierNoopPass(Registry);
  initializeEliminateAvailableExternallyPass(Registry);
}

void LLVM37InitializeIPO(LLVM37PassRegistryRef R) {
  initializeIPO(*unwrap(R));
}

void LLVM37AddArgumentPromotionPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createArgumentPromotionPass());
}

void LLVM37AddConstantMergePass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createConstantMergePass());
}

void LLVM37AddDeadArgEliminationPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createDeadArgEliminationPass());
}

void LLVM37AddFunctionAttrsPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createFunctionAttrsPass());
}

void LLVM37AddFunctionInliningPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createFunctionInliningPass());
}

void LLVM37AddAlwaysInlinerPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(llvm37::createAlwaysInlinerPass());
}

void LLVM37AddGlobalDCEPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createGlobalDCEPass());
}

void LLVM37AddGlobalOptimizerPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createGlobalOptimizerPass());
}

void LLVM37AddIPConstantPropagationPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createIPConstantPropagationPass());
}

void LLVM37AddPruneEHPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createPruneEHPass());
}

void LLVM37AddIPSCCPPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createIPSCCPPass());
}

void LLVM37AddInternalizePass(LLVM37PassManagerRef PM, unsigned AllButMain) {
  std::vector<const char *> Export;
  if (AllButMain)
    Export.push_back("main");
  unwrap(PM)->add(createInternalizePass(Export));
}

void LLVM37AddStripDeadPrototypesPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createStripDeadPrototypesPass());
}

void LLVM37AddStripSymbolsPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createStripSymbolsPass());
}
