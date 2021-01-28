/*===-- Scalar.h - Scalar Transformation Library C Interface ----*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37ScalarOpts.a, which         *|
|* implements various scalar transformations of the LLVM37 IR.                  *|
|*                                                                            *|
|* Many exotic languages can interoperate with C code but have a harder time  *|
|* with C++ due to name mangling. So in addition to C, this interface enables *|
|* tools written in such languages.                                           *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_TRANSFORMS_SCALAR_H
#define LLVM37_C_TRANSFORMS_SCALAR_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CTransformsScalar Scalar transformations
 * @ingroup LLVM37CTransforms
 *
 * @{
 */

/** See llvm37::createAggressiveDCEPass function. */
void LLVM37AddAggressiveDCEPass(LLVM37PassManagerRef PM);

/** See llvm37::createBitTrackingDCEPass function. */
void LLVM37AddBitTrackingDCEPass(LLVM37PassManagerRef PM);

/** See llvm37::createAlignmentFromAssumptionsPass function. */
void LLVM37AddAlignmentFromAssumptionsPass(LLVM37PassManagerRef PM);

/** See llvm37::createCFGSimplificationPass function. */
void LLVM37AddCFGSimplificationPass(LLVM37PassManagerRef PM);

/** See llvm37::createDeadStoreEliminationPass function. */
void LLVM37AddDeadStoreEliminationPass(LLVM37PassManagerRef PM);

/** See llvm37::createScalarizerPass function. */
void LLVM37AddScalarizerPass(LLVM37PassManagerRef PM);

/** See llvm37::createMergedLoadStoreMotionPass function. */
void LLVM37AddMergedLoadStoreMotionPass(LLVM37PassManagerRef PM);

/** See llvm37::createGVNPass function. */
void LLVM37AddGVNPass(LLVM37PassManagerRef PM);

/** See llvm37::createIndVarSimplifyPass function. */
void LLVM37AddIndVarSimplifyPass(LLVM37PassManagerRef PM);

/** See llvm37::createInstructionCombiningPass function. */
void LLVM37AddInstructionCombiningPass(LLVM37PassManagerRef PM);

/** See llvm37::createJumpThreadingPass function. */
void LLVM37AddJumpThreadingPass(LLVM37PassManagerRef PM);

/** See llvm37::createLICMPass function. */
void LLVM37AddLICMPass(LLVM37PassManagerRef PM);

/** See llvm37::createLoopDeletionPass function. */
void LLVM37AddLoopDeletionPass(LLVM37PassManagerRef PM);

/** See llvm37::createLoopIdiomPass function */
void LLVM37AddLoopIdiomPass(LLVM37PassManagerRef PM);

/** See llvm37::createLoopRotatePass function. */
void LLVM37AddLoopRotatePass(LLVM37PassManagerRef PM);

/** See llvm37::createLoopRerollPass function. */
void LLVM37AddLoopRerollPass(LLVM37PassManagerRef PM);

/** See llvm37::createLoopUnrollPass function. */
void LLVM37AddLoopUnrollPass(LLVM37PassManagerRef PM);

/** See llvm37::createLoopUnswitchPass function. */
void LLVM37AddLoopUnswitchPass(LLVM37PassManagerRef PM);

/** See llvm37::createMemCpyOptPass function. */
void LLVM37AddMemCpyOptPass(LLVM37PassManagerRef PM);

/** See llvm37::createPartiallyInlineLibCallsPass function. */
void LLVM37AddPartiallyInlineLibCallsPass(LLVM37PassManagerRef PM);

/** See llvm37::createLowerSwitchPass function. */
void LLVM37AddLowerSwitchPass(LLVM37PassManagerRef PM);

/** See llvm37::createPromoteMemoryToRegisterPass function. */
void LLVM37AddPromoteMemoryToRegisterPass(LLVM37PassManagerRef PM);

/** See llvm37::createReassociatePass function. */
void LLVM37AddReassociatePass(LLVM37PassManagerRef PM);

/** See llvm37::createSCCPPass function. */
void LLVM37AddSCCPPass(LLVM37PassManagerRef PM);

/** See llvm37::createScalarReplAggregatesPass function. */
void LLVM37AddScalarReplAggregatesPass(LLVM37PassManagerRef PM);

/** See llvm37::createScalarReplAggregatesPass function. */
void LLVM37AddScalarReplAggregatesPassSSA(LLVM37PassManagerRef PM);

/** See llvm37::createScalarReplAggregatesPass function. */
void LLVM37AddScalarReplAggregatesPassWithThreshold(LLVM37PassManagerRef PM,
                                                  int Threshold);

/** See llvm37::createSimplifyLibCallsPass function. */
void LLVM37AddSimplifyLibCallsPass(LLVM37PassManagerRef PM);

/** See llvm37::createTailCallEliminationPass function. */
void LLVM37AddTailCallEliminationPass(LLVM37PassManagerRef PM);

/** See llvm37::createConstantPropagationPass function. */
void LLVM37AddConstantPropagationPass(LLVM37PassManagerRef PM);

/** See llvm37::demotePromoteMemoryToRegisterPass function. */
void LLVM37AddDemoteMemoryToRegisterPass(LLVM37PassManagerRef PM);

/** See llvm37::createVerifierPass function. */
void LLVM37AddVerifierPass(LLVM37PassManagerRef PM);

/** See llvm37::createCorrelatedValuePropagationPass function */
void LLVM37AddCorrelatedValuePropagationPass(LLVM37PassManagerRef PM);

/** See llvm37::createEarlyCSEPass function */
void LLVM37AddEarlyCSEPass(LLVM37PassManagerRef PM);

/** See llvm37::createLowerExpectIntrinsicPass function */
void LLVM37AddLowerExpectIntrinsicPass(LLVM37PassManagerRef PM);

/** See llvm37::createTypeBasedAliasAnalysisPass function */
void LLVM37AddTypeBasedAliasAnalysisPass(LLVM37PassManagerRef PM);

/** See llvm37::createScopedNoAliasAAPass function */
void LLVM37AddScopedNoAliasAAPass(LLVM37PassManagerRef PM);

/** See llvm37::createBasicAliasAnalysisPass function */
void LLVM37AddBasicAliasAnalysisPass(LLVM37PassManagerRef PM);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif
