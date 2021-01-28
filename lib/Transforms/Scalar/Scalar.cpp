//===-- Scalar.cpp --------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements common infrastructure for libLLVM37ScalarOpts.a, which
// implements several scalar transformations over the LLVM37 intermediate
// representation, including the C bindings for that library.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Transforms/Scalar.h"
#include "llvm37-c/Initialization.h"
#include "llvm37-c/Transforms/Scalar.h"
#include "llvm37/Analysis/Passes.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/IR/Verifier.h"
#include "llvm37/InitializePasses.h"
#include "llvm37/IR/LegacyPassManager.h"

using namespace llvm37;

/// initializeScalarOptsPasses - Initialize all passes linked into the
/// ScalarOpts library.
void llvm37::initializeScalarOpts(PassRegistry &Registry) {
  initializeADCEPass(Registry);
  initializeBDCEPass(Registry);
  initializeAlignmentFromAssumptionsPass(Registry);
  initializeSampleProfileLoaderPass(Registry);
  initializeConstantHoistingPass(Registry);
  initializeConstantPropagationPass(Registry);
  initializeCorrelatedValuePropagationPass(Registry);
  initializeDCEPass(Registry);
  initializeDeadInstEliminationPass(Registry);
  initializeScalarizerPass(Registry);
  initializeDSEPass(Registry);
  initializeGVNPass(Registry);
  initializeEarlyCSELegacyPassPass(Registry);
  initializeFlattenCFGPassPass(Registry);
  initializeInductiveRangeCheckEliminationPass(Registry);
  initializeIndVarSimplifyPass(Registry);
  initializeJumpThreadingPass(Registry);
  initializeLICMPass(Registry);
  initializeLoopDeletionPass(Registry);
  initializeLoopAccessAnalysisPass(Registry);
  initializeLoopInstSimplifyPass(Registry);
  initializeLoopInterchangePass(Registry);
  initializeLoopRotatePass(Registry);
  initializeLoopStrengthReducePass(Registry);
  initializeLoopRerollPass(Registry);
  initializeLoopUnrollPass(Registry);
  initializeLoopUnswitchPass(Registry);
  initializeLoopIdiomRecognizePass(Registry);
  initializeLowerAtomicPass(Registry);
  initializeLowerExpectIntrinsicPass(Registry);
  initializeMemCpyOptPass(Registry);
  initializeMergedLoadStoreMotionPass(Registry);
  initializeNaryReassociatePass(Registry);
  initializePartiallyInlineLibCallsPass(Registry);
  initializeReassociatePass(Registry);
  initializeRegToMemPass(Registry);
  initializeRegToMemHlslPass(Registry); // HLSL Change
  initializeRewriteStatepointsForGCPass(Registry);
  initializeSCCPPass(Registry);
  initializeIPSCCPPass(Registry);
  initializeSROAPass(Registry);
  initializeSROA_DTPass(Registry);
  initializeSROA_SSAUpPass(Registry);
  initializeCFGSimplifyPassPass(Registry);
  initializeStructurizeCFGPass(Registry);
  initializeSinkingPass(Registry);
  initializeTailCallElimPass(Registry);
  initializeSeparateConstOffsetFromGEPPass(Registry);
  initializeSpeculativeExecutionPass(Registry);
  initializeStraightLineStrengthReducePass(Registry);
  initializeLoadCombinePass(Registry);
  initializePlaceBackedgeSafepointsImplPass(Registry);
  initializePlaceSafepointsPass(Registry);
  initializeFloat2IntPass(Registry);
  initializeLoopDistributePass(Registry);
}

void LLVM37InitializeScalarOpts(LLVM37PassRegistryRef R) {
  initializeScalarOpts(*unwrap(R));
}

void LLVM37AddAggressiveDCEPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createAggressiveDCEPass());
}

void LLVM37AddBitTrackingDCEPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createBitTrackingDCEPass());
}

void LLVM37AddAlignmentFromAssumptionsPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createAlignmentFromAssumptionsPass());
}

void LLVM37AddCFGSimplificationPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createCFGSimplificationPass());
}

void LLVM37AddDeadStoreEliminationPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createDeadStoreEliminationPass());
}

void LLVM37AddScalarizerPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createScalarizerPass());
}

void LLVM37AddGVNPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createGVNPass());
}

void LLVM37AddMergedLoadStoreMotionPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createMergedLoadStoreMotionPass());
}

void LLVM37AddIndVarSimplifyPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createIndVarSimplifyPass());
}

void LLVM37AddInstructionCombiningPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createInstructionCombiningPass());
}

void LLVM37AddJumpThreadingPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createJumpThreadingPass());
}

void LLVM37AddLICMPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLICMPass());
}

void LLVM37AddLoopDeletionPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLoopDeletionPass());
}

void LLVM37AddLoopIdiomPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLoopIdiomPass());
}

void LLVM37AddLoopRotatePass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLoopRotatePass());
}

void LLVM37AddLoopRerollPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLoopRerollPass());
}

void LLVM37AddLoopUnrollPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLoopUnrollPass());
}

void LLVM37AddLoopUnswitchPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLoopUnswitchPass());
}

void LLVM37AddMemCpyOptPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createMemCpyOptPass());
}

void LLVM37AddPartiallyInlineLibCallsPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createPartiallyInlineLibCallsPass());
}

void LLVM37AddLowerSwitchPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLowerSwitchPass());
}

void LLVM37AddPromoteMemoryToRegisterPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createPromoteMemoryToRegisterPass());
}

void LLVM37AddReassociatePass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createReassociatePass());
}

void LLVM37AddSCCPPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createSCCPPass());
}

void LLVM37AddScalarReplAggregatesPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createScalarReplAggregatesPass());
}

void LLVM37AddScalarReplAggregatesPassSSA(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createScalarReplAggregatesPass(-1, false));
}

void LLVM37AddScalarReplAggregatesPassWithThreshold(LLVM37PassManagerRef PM,
                                                  int Threshold) {
  unwrap(PM)->add(createScalarReplAggregatesPass(Threshold));
}

void LLVM37AddSimplifyLibCallsPass(LLVM37PassManagerRef PM) {
  // NOTE: The simplify-libcalls pass has been removed.
}

void LLVM37AddTailCallEliminationPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createTailCallEliminationPass());
}

void LLVM37AddConstantPropagationPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createConstantPropagationPass());
}

void LLVM37AddDemoteMemoryToRegisterPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createDemoteRegisterToMemoryPass());
}

// HLSL Change start
void LLVM37AddDemoteMemoryToRegisterHlslPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createDemoteRegisterToMemoryHlslPass());
}
// HLSL Change end

void LLVM37AddVerifierPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createVerifierPass());
}

void LLVM37AddCorrelatedValuePropagationPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createCorrelatedValuePropagationPass());
}

void LLVM37AddEarlyCSEPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createEarlyCSEPass());
}

void LLVM37AddTypeBasedAliasAnalysisPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createTypeBasedAliasAnalysisPass());
}

void LLVM37AddScopedNoAliasAAPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createScopedNoAliasAAPass());
}

void LLVM37AddBasicAliasAnalysisPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createBasicAliasAnalysisPass());
}

void LLVM37AddLowerExpectIntrinsicPass(LLVM37PassManagerRef PM) {
  unwrap(PM)->add(createLowerExpectIntrinsicPass());
}
