//===-- CodeGen.cpp -------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the common initialization routines for the
// CodeGen library.
//
//===----------------------------------------------------------------------===//

#include "llvm37/InitializePasses.h"
#include "llvm37-c/Initialization.h"
#include "llvm37/PassRegistry.h"

using namespace llvm37;

/// initializeCodeGen - Initialize all passes linked into the CodeGen library.
void llvm37::initializeCodeGen(PassRegistry &Registry) {
  initializeAtomicExpandPass(Registry);
  initializeBranchFolderPassPass(Registry);
  initializeCodeGenPreparePass(Registry);
  initializeDeadMachineInstructionElimPass(Registry);
  initializeDwarfEHPreparePass(Registry);
  initializeEarlyIfConverterPass(Registry);
  initializeExpandISelPseudosPass(Registry);
  initializeExpandPostRAPass(Registry);
  initializeFinalizeMachineBundlesPass(Registry);
  initializeGCMachineCodeAnalysisPass(Registry);
  initializeGCModuleInfoPass(Registry);
  initializeIfConverterPass(Registry);
  initializeLiveDebugVariablesPass(Registry);
  initializeLiveIntervalsPass(Registry);
  initializeLiveStacksPass(Registry);
  initializeLiveVariablesPass(Registry);
  initializeLocalStackSlotPassPass(Registry);
  initializeLowerIntrinsicsPass(Registry);
  initializeMachineBlockFrequencyInfoPass(Registry);
  initializeMachineBlockPlacementPass(Registry);
  initializeMachineBlockPlacementStatsPass(Registry);
  initializeMachineCSEPass(Registry);
  initializeImplicitNullChecksPass(Registry);
  initializeMachineCombinerPass(Registry);
  initializeMachineCopyPropagationPass(Registry);
  initializeMachineDominatorTreePass(Registry);
  initializeMachineFunctionPrinterPassPass(Registry);
  initializeMachineLICMPass(Registry);
  initializeMachineLoopInfoPass(Registry);
  initializeMachineModuleInfoPass(Registry);
  initializeMachinePostDominatorTreePass(Registry);
  initializeMachineSchedulerPass(Registry);
  initializeMachineSinkingPass(Registry);
  initializeMachineVerifierPassPass(Registry);
  initializeOptimizePHIsPass(Registry);
  initializePEIPass(Registry);
  initializePHIEliminationPass(Registry);
  initializePeepholeOptimizerPass(Registry);
  initializePostMachineSchedulerPass(Registry);
  initializePostRASchedulerPass(Registry);
  initializeProcessImplicitDefsPass(Registry);
  initializeRegisterCoalescerPass(Registry);
  initializeShrinkWrapPass(Registry);
  initializeSlotIndexesPass(Registry);
  initializeStackColoringPass(Registry);
  initializeStackMapLivenessPass(Registry);
  initializeStackProtectorPass(Registry);
  initializeStackSlotColoringPass(Registry);
  initializeTailDuplicatePassPass(Registry);
  initializeTargetPassConfigPass(Registry);
  initializeTwoAddressInstructionPassPass(Registry);
  initializeUnpackMachineBundlesPass(Registry);
  initializeUnreachableBlockElimPass(Registry);
  initializeUnreachableMachineBlockElimPass(Registry);
  initializeVirtRegMapPass(Registry);
  initializeVirtRegRewriterPass(Registry);
  initializeWinEHPreparePass(Registry);
}

void LLVM37InitializeCodeGen(LLVM37PassRegistryRef R) {
  initializeCodeGen(*unwrap(R));
}
