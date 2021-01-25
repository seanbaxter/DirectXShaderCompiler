/*===-- Scalar.h - Scalar Transformation Library C Interface ----*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVMScalarOpts.a, which         *|
|* implements various scalar transformations of the LLVM IR.                  *|
|*                                                                            *|
|* Many exotic languages can interoperate with C code but have a harder time  *|
|* with C++ due to name mangling. So in addition to C, this interface enables *|
|* tools written in such languages.                                           *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_TRANSFORMS_SCALAR_H
#define LLVM37_C_TRANSFORMS_SCALAR_H

#include "llvm-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVMCTransformsScalar Scalar transformations
 * @ingroup LLVMCTransforms
 *
 * @{
 */

/** See llvm37::createAggressiveDCEPass function. */
void LLVMAddAggressiveDCEPass(LLVMPassManagerRef PM);

/** See llvm37::createBitTrackingDCEPass function. */
void LLVMAddBitTrackingDCEPass(LLVMPassManagerRef PM);

/** See llvm37::createAlignmentFromAssumptionsPass function. */
void LLVMAddAlignmentFromAssumptionsPass(LLVMPassManagerRef PM);

/** See llvm37::createCFGSimplificationPass function. */
void LLVMAddCFGSimplificationPass(LLVMPassManagerRef PM);

/** See llvm37::createDeadStoreEliminationPass function. */
void LLVMAddDeadStoreEliminationPass(LLVMPassManagerRef PM);

/** See llvm37::createScalarizerPass function. */
void LLVMAddScalarizerPass(LLVMPassManagerRef PM);

/** See llvm37::createMergedLoadStoreMotionPass function. */
void LLVMAddMergedLoadStoreMotionPass(LLVMPassManagerRef PM);

/** See llvm37::createGVNPass function. */
void LLVMAddGVNPass(LLVMPassManagerRef PM);

/** See llvm37::createIndVarSimplifyPass function. */
void LLVMAddIndVarSimplifyPass(LLVMPassManagerRef PM);

/** See llvm37::createInstructionCombiningPass function. */
void LLVMAddInstructionCombiningPass(LLVMPassManagerRef PM);

/** See llvm37::createJumpThreadingPass function. */
void LLVMAddJumpThreadingPass(LLVMPassManagerRef PM);

/** See llvm37::createLICMPass function. */
void LLVMAddLICMPass(LLVMPassManagerRef PM);

/** See llvm37::createLoopDeletionPass function. */
void LLVMAddLoopDeletionPass(LLVMPassManagerRef PM);

/** See llvm37::createLoopIdiomPass function */
void LLVMAddLoopIdiomPass(LLVMPassManagerRef PM);

/** See llvm37::createLoopRotatePass function. */
void LLVMAddLoopRotatePass(LLVMPassManagerRef PM);

/** See llvm37::createLoopRerollPass function. */
void LLVMAddLoopRerollPass(LLVMPassManagerRef PM);

/** See llvm37::createLoopUnrollPass function. */
void LLVMAddLoopUnrollPass(LLVMPassManagerRef PM);

/** See llvm37::createLoopUnswitchPass function. */
void LLVMAddLoopUnswitchPass(LLVMPassManagerRef PM);

/** See llvm37::createMemCpyOptPass function. */
void LLVMAddMemCpyOptPass(LLVMPassManagerRef PM);

/** See llvm37::createPartiallyInlineLibCallsPass function. */
void LLVMAddPartiallyInlineLibCallsPass(LLVMPassManagerRef PM);

/** See llvm37::createLowerSwitchPass function. */
void LLVMAddLowerSwitchPass(LLVMPassManagerRef PM);

/** See llvm37::createPromoteMemoryToRegisterPass function. */
void LLVMAddPromoteMemoryToRegisterPass(LLVMPassManagerRef PM);

/** See llvm37::createReassociatePass function. */
void LLVMAddReassociatePass(LLVMPassManagerRef PM);

/** See llvm37::createSCCPPass function. */
void LLVMAddSCCPPass(LLVMPassManagerRef PM);

/** See llvm37::createScalarReplAggregatesPass function. */
void LLVMAddScalarReplAggregatesPass(LLVMPassManagerRef PM);

/** See llvm37::createScalarReplAggregatesPass function. */
void LLVMAddScalarReplAggregatesPassSSA(LLVMPassManagerRef PM);

/** See llvm37::createScalarReplAggregatesPass function. */
void LLVMAddScalarReplAggregatesPassWithThreshold(LLVMPassManagerRef PM,
                                                  int Threshold);

/** See llvm37::createSimplifyLibCallsPass function. */
void LLVMAddSimplifyLibCallsPass(LLVMPassManagerRef PM);

/** See llvm37::createTailCallEliminationPass function. */
void LLVMAddTailCallEliminationPass(LLVMPassManagerRef PM);

/** See llvm37::createConstantPropagationPass function. */
void LLVMAddConstantPropagationPass(LLVMPassManagerRef PM);

/** See llvm37::demotePromoteMemoryToRegisterPass function. */
void LLVMAddDemoteMemoryToRegisterPass(LLVMPassManagerRef PM);

/** See llvm37::createVerifierPass function. */
void LLVMAddVerifierPass(LLVMPassManagerRef PM);

/** See llvm37::createCorrelatedValuePropagationPass function */
void LLVMAddCorrelatedValuePropagationPass(LLVMPassManagerRef PM);

/** See llvm37::createEarlyCSEPass function */
void LLVMAddEarlyCSEPass(LLVMPassManagerRef PM);

/** See llvm37::createLowerExpectIntrinsicPass function */
void LLVMAddLowerExpectIntrinsicPass(LLVMPassManagerRef PM);

/** See llvm37::createTypeBasedAliasAnalysisPass function */
void LLVMAddTypeBasedAliasAnalysisPass(LLVMPassManagerRef PM);

/** See llvm37::createScopedNoAliasAAPass function */
void LLVMAddScopedNoAliasAAPass(LLVMPassManagerRef PM);

/** See llvm37::createBasicAliasAnalysisPass function */
void LLVMAddBasicAliasAnalysisPass(LLVMPassManagerRef PM);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif
