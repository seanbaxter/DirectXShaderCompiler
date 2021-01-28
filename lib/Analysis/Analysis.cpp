//===-- Analysis.cpp ------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/Analysis.h"
#include "llvm37-c/Initialization.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/Verifier.h"
#include "llvm37/InitializePasses.h"
#include "llvm37/PassRegistry.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstring>

using namespace llvm37;

/// initializeAnalysis - Initialize all passes linked into the Analysis library.
void llvm37::initializeAnalysis(PassRegistry &Registry) {
  initializeAliasAnalysisAnalysisGroup(Registry);
  initializeAliasAnalysisCounterPass(Registry);
  initializeAAEvalPass(Registry);
  initializeAliasDebuggerPass(Registry);
  initializeAliasSetPrinterPass(Registry);
  initializeNoAAPass(Registry);
  initializeBasicAliasAnalysisPass(Registry);
  initializeBlockFrequencyInfoPass(Registry);
  initializeBranchProbabilityInfoPass(Registry);
  initializeCostModelAnalysisPass(Registry);
  initializeCFGViewerPass(Registry);
  initializeCFGPrinterPass(Registry);
  initializeCFGOnlyViewerPass(Registry);
  initializeCFGOnlyPrinterPass(Registry);
  initializeCFLAliasAnalysisPass(Registry);
  initializeDependenceAnalysisPass(Registry);
  initializeDelinearizationPass(Registry);
  initializeDivergenceAnalysisPass(Registry);
  initializeDominanceFrontierPass(Registry);
  initializeDomViewerPass(Registry);
  initializeDomPrinterPass(Registry);
  initializeDomOnlyViewerPass(Registry);
  initializePostDomViewerPass(Registry);
  initializeDomOnlyPrinterPass(Registry);
  initializePostDomPrinterPass(Registry);
  initializePostDomOnlyViewerPass(Registry);
  initializePostDomOnlyPrinterPass(Registry);
  initializeIVUsersPass(Registry);
  initializeInstCountPass(Registry);
  initializeIntervalPartitionPass(Registry);
  initializeLazyValueInfoPass(Registry);
  initializeLibCallAliasAnalysisPass(Registry);
  initializeLintPass(Registry);
  initializeLoopInfoWrapperPassPass(Registry);
  initializeMemDepPrinterPass(Registry);
  initializeMemDerefPrinterPass(Registry);
  initializeMemoryDependenceAnalysisPass(Registry);
  initializeModuleDebugInfoPrinterPass(Registry);
  initializePostDominatorTreePass(Registry);
  initializeRegionInfoPassPass(Registry);
  initializeRegionViewerPass(Registry);
  initializeRegionPrinterPass(Registry);
  initializeRegionOnlyViewerPass(Registry);
  initializeRegionOnlyPrinterPass(Registry);
  initializeScalarEvolutionPass(Registry);
  initializeScalarEvolutionAliasAnalysisPass(Registry);
  initializeTargetTransformInfoWrapperPassPass(Registry);
  initializeTypeBasedAliasAnalysisPass(Registry);
  initializeScopedNoAliasAAPass(Registry);
}

void LLVM37InitializeAnalysis(LLVM37PassRegistryRef R) {
  initializeAnalysis(*unwrap(R));
}

_Use_decl_annotations_
LLVM37Bool LLVM37VerifyModule(LLVM37ModuleRef M, LLVM37VerifierFailureAction Action,
                          char **OutMessages) {
  raw_ostream *DebugOS = Action != LLVM37ReturnStatusAction ? &errs() : nullptr;
  std::string Messages;
  raw_string_ostream MsgsOS(Messages);

  LLVM37Bool Result = verifyModule(*unwrap(M), OutMessages ? &MsgsOS : DebugOS);

  // Duplicate the output to stderr.
  if (DebugOS && OutMessages)
    *DebugOS << MsgsOS.str();

  if (Action == LLVM37AbortProcessAction && Result)
    report_fatal_error("Broken module found, compilation aborted!");

  if (OutMessages)
    *OutMessages = _strdup(MsgsOS.str().c_str()); // HLSL Change for strdup

  return Result;
}

LLVM37Bool LLVM37VerifyFunction(LLVM37ValueRef Fn, LLVM37VerifierFailureAction Action) {
  LLVM37Bool Result = verifyFunction(
      *unwrap<Function>(Fn), Action != LLVM37ReturnStatusAction ? &errs()
                                                              : nullptr);

  if (Action == LLVM37AbortProcessAction && Result)
    report_fatal_error("Broken function found, compilation aborted!");

  return Result;
}

void LLVM37ViewFunctionCFG(LLVM37ValueRef Fn) {
  Function *F = unwrap<Function>(Fn);
  F->viewCFG();
}

void LLVM37ViewFunctionCFGOnly(LLVM37ValueRef Fn) {
  Function *F = unwrap<Function>(Fn);
  F->viewCFGOnly();
}
