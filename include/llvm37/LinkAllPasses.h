//===- llvm/LinkAllPasses.h ------------ Reference All Passes ---*- C++ -*-===//
//
//                      The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This header file pulls in all transformation and analysis passes for tools
// like opt and bugpoint that need this functionality.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LINKALLPASSES_H
#define LLVM37_LINKALLPASSES_H

#include "llvm37/ADT/Statistic.h"
#include "llvm37/Analysis/AliasAnalysis.h"
#include "llvm37/Analysis/AliasSetTracker.h"
#include "llvm37/Analysis/CallPrinter.h"
#include "llvm37/Analysis/DomPrinter.h"
#include "llvm37/Analysis/IntervalPartition.h"
#include "llvm37/Analysis/Lint.h"
#include "llvm37/Analysis/Passes.h"
#include "llvm37/Analysis/PostDominators.h"
#include "llvm37/Analysis/RegionPass.h"
#include "llvm37/Analysis/RegionPrinter.h"
#include "llvm37/Analysis/ScalarEvolution.h"
#include "llvm37/CodeGen/Passes.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/IRPrintingPasses.h"
#include "llvm37/Transforms/IPO.h"
#include "llvm37/Transforms/Instrumentation.h"
#include "llvm37/Transforms/ObjCARC.h"
#include "llvm37/Transforms/Scalar.h"
#include "llvm37/Transforms/Utils/SymbolRewriter.h"
#include "llvm37/Transforms/Utils/UnifyFunctionExitNodes.h"
#include "llvm37/Transforms/Vectorize.h"
#include "llvm37/Support/Valgrind.h"
#include <cstdlib>

namespace {
  struct ForcePassLinking {
    ForcePassLinking() {
      // We must reference the passes in such a way that compilers will not
      // delete it all as dead code, even with whole program optimization,
      // yet is effectively a NO-OP. As the compiler isn't smart enough
      // to know that getenv() never returns -1, this will do the job.
      if (std::getenv("bar") != (char*) -1)
        return;

      (void) llvm37::createAAEvalPass();
      (void) llvm37::createAggressiveDCEPass();
      (void) llvm37::createBitTrackingDCEPass();
      (void) llvm37::createAliasAnalysisCounterPass();
      (void) llvm37::createAliasDebugger();
      (void) llvm37::createArgumentPromotionPass();
      (void) llvm37::createAlignmentFromAssumptionsPass();
      (void) llvm37::createBasicAliasAnalysisPass();
      (void) llvm37::createLibCallAliasAnalysisPass(nullptr);
      (void) llvm37::createScalarEvolutionAliasAnalysisPass();
      (void) llvm37::createTypeBasedAliasAnalysisPass();
      (void) llvm37::createScopedNoAliasAAPass();
      // (void) llvm37::createBoundsCheckingPass(); // HLSL Change - do not link
      (void) llvm37::createBreakCriticalEdgesPass();
      (void) llvm37::createCallGraphPrinterPass();
      (void) llvm37::createCallGraphViewerPass();
      (void) llvm37::createCFGSimplificationPass();
      (void) llvm37::createCFLAliasAnalysisPass();
      (void) llvm37::createStructurizeCFGPass();
      (void) llvm37::createConstantMergePass();
      (void) llvm37::createConstantPropagationPass();
      (void) llvm37::createCostModelAnalysisPass();
      (void) llvm37::createDeadArgEliminationPass();
      (void) llvm37::createDeadCodeEliminationPass();
      (void) llvm37::createDeadInstEliminationPass();
      (void) llvm37::createDeadStoreEliminationPass();
      (void) llvm37::createDependenceAnalysisPass();
      (void) llvm37::createDivergenceAnalysisPass();
      (void) llvm37::createDomOnlyPrinterPass();
      (void) llvm37::createDomPrinterPass();
      (void) llvm37::createDomOnlyViewerPass();
      (void) llvm37::createDomViewerPass();
      // (void) llvm37::createGCOVProfilerPass(); // HLSL Change - do not link
      // (void) llvm37::createInstrProfilingPass(); // HLSL Change - do not link
      (void) llvm37::createFunctionInliningPass();
      (void) llvm37::createAlwaysInlinerPass();
      (void) llvm37::createGlobalDCEPass();
      (void) llvm37::createGlobalOptimizerPass();
      (void) llvm37::createGlobalsModRefPass();
      (void) llvm37::createIPConstantPropagationPass();
      (void) llvm37::createIPSCCPPass();
      (void) llvm37::createInductiveRangeCheckEliminationPass();
      (void) llvm37::createIndVarSimplifyPass();
      (void) llvm37::createInstructionCombiningPass();
      (void) llvm37::createInternalizePass();
      (void) llvm37::createLCSSAPass();
      (void) llvm37::createLICMPass();
      (void) llvm37::createLazyValueInfoPass();
      (void) llvm37::createLoopExtractorPass();
      (void) llvm37::createLoopInterchangePass();
      (void) llvm37::createLoopSimplifyPass();
      (void) llvm37::createLoopStrengthReducePass();
      (void) llvm37::createLoopRerollPass();
      (void) llvm37::createLoopUnrollPass();
      (void) llvm37::createLoopUnswitchPass();
      (void) llvm37::createLoopIdiomPass();
      (void) llvm37::createLoopRotatePass();
      (void) llvm37::createLowerExpectIntrinsicPass();
      (void) llvm37::createLowerInvokePass();
      (void) llvm37::createLowerSwitchPass();
      (void) llvm37::createNaryReassociatePass();
      (void) llvm37::createNoAAPass();
      // (void) llvm37::createObjCARCAliasAnalysisPass(); // HLSL Change - do not link
      // (void) llvm37::createObjCARCAPElimPass(); // HLSL Change - do not link
      // (void) llvm37::createObjCARCExpandPass(); // HLSL Change - do not link
      // (void) llvm37::createObjCARCContractPass(); // HLSL Change - do not link
      // (void) llvm37::createObjCARCOptPass(); // HLSL Change - do not link
      // (void) llvm37::createPAEvalPass();// HLSL Change - do not link
      (void) llvm37::createPromoteMemoryToRegisterPass();
      (void) llvm37::createDemoteRegisterToMemoryPass();
      // (void) llvm37::createPruneEHPass(); // HLSL Change - do not link
      (void) llvm37::createPostDomOnlyPrinterPass();
      (void) llvm37::createPostDomPrinterPass();
      (void) llvm37::createPostDomOnlyViewerPass();
      (void) llvm37::createPostDomViewerPass();
      (void) llvm37::createReassociatePass();
      (void) llvm37::createRegionInfoPass();
      (void) llvm37::createRegionOnlyPrinterPass();
      (void) llvm37::createRegionOnlyViewerPass();
      (void) llvm37::createRegionPrinterPass();
      (void) llvm37::createRegionViewerPass();
      (void) llvm37::createSCCPPass();
      // (void) llvm37::createSafeStackPass(); // HLSL Change - do not link
      (void) llvm37::createScalarReplAggregatesPass();
      (void) llvm37::createSingleLoopExtractorPass();
      (void) llvm37::createStripSymbolsPass();
      (void) llvm37::createStripNonDebugSymbolsPass();
      (void) llvm37::createStripDeadDebugInfoPass();
      (void) llvm37::createStripDeadPrototypesPass();
      (void) llvm37::createTailCallEliminationPass();
      (void) llvm37::createJumpThreadingPass();
      (void) llvm37::createUnifyFunctionExitNodesPass();
      (void) llvm37::createInstCountPass();
      (void) llvm37::createConstantHoistingPass();
      // (void) llvm37::createCodeGenPreparePass(); // HLSL Change - do not link
      (void) llvm37::createEarlyCSEPass();
      (void) llvm37::createMergedLoadStoreMotionPass();
      (void) llvm37::createGVNPass();
      (void) llvm37::createMemCpyOptPass();
      (void) llvm37::createLoopDeletionPass();
      (void) llvm37::createPostDomTree();
      (void) llvm37::createInstructionNamerPass();
      (void) llvm37::createMetaRenamerPass();
      (void) llvm37::createFunctionAttrsPass();
      (void) llvm37::createMergeFunctionsPass();
      // HLSL Change Begin - avoid casting nullptrs
      std::string buf;
      llvm37::raw_string_ostream os(buf);
      (void) llvm37::createPrintModulePass(os);
      (void) llvm37::createPrintFunctionPass(os);
      (void) llvm37::createPrintBasicBlockPass(os);
      // HLSL Change End - avoid casting nullptrs
      (void) llvm37::createModuleDebugInfoPrinterPass();
      (void) llvm37::createPartialInliningPass();
      (void) llvm37::createLintPass();
      (void) llvm37::createSinkingPass();
      (void) llvm37::createLowerAtomicPass();
      (void) llvm37::createCorrelatedValuePropagationPass();
      (void) llvm37::createMemDepPrinter();
      (void) llvm37::createInstructionSimplifierPass();
      // (void) llvm37::createLoopVectorizePass(); // HLSL Change - do not link
      // (void) llvm37::createSLPVectorizerPass(); // HLSL Change - do not link
      // (void) llvm37::createBBVectorizePass(); // HLSL Change - do not link
      (void) llvm37::createPartiallyInlineLibCallsPass();
      (void) llvm37::createScalarizerPass();
      (void) llvm37::createSeparateConstOffsetFromGEPPass();
      (void) llvm37::createSpeculativeExecutionPass();
      (void) llvm37::createRewriteSymbolsPass();
      (void) llvm37::createStraightLineStrengthReducePass();
      (void) llvm37::createMemDerefPrinter();
      (void) llvm37::createFloat2IntPass();
      (void) llvm37::createEliminateAvailableExternallyPass();

      (void)new llvm37::IntervalPartition();
      (void)new llvm37::ScalarEvolution();
      // HLSL Change Begin - avoid casting nullptrs
      llvm37::Function::Create(nullptr, llvm37::GlobalValue::ExternalLinkage)->viewCFGOnly();
      llvm37::AliasAnalysis AA;
      llvm37::AliasSetTracker X(AA);
      // HLSL Change End - avoid casting nullptrs
      X.add(nullptr, 0, llvm37::AAMDNodes()); // for -print-alias-sets
      (void) llvm37::AreStatisticsEnabled();
      (void) llvm37::sys::RunningOnValgrind();
    }
  } ForcePassLinking; // Force link by creating a global definition.
}

#endif
