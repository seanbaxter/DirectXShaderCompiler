/*===-- IPO.h - Interprocedural Transformations C Interface -----*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37IPO.a, which implements     *|
|* various interprocedural transformations of the LLVM37 IR.                    *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_TRANSFORMS_IPO_H
#define LLVM37_C_TRANSFORMS_IPO_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CTransformsIPO Interprocedural transformations
 * @ingroup LLVM37CTransforms
 *
 * @{
 */

/** See llvm37::createArgumentPromotionPass function. */
void LLVM37AddArgumentPromotionPass(LLVM37PassManagerRef PM);

/** See llvm37::createConstantMergePass function. */
void LLVM37AddConstantMergePass(LLVM37PassManagerRef PM);

/** See llvm37::createDeadArgEliminationPass function. */
void LLVM37AddDeadArgEliminationPass(LLVM37PassManagerRef PM);

/** See llvm37::createFunctionAttrsPass function. */
void LLVM37AddFunctionAttrsPass(LLVM37PassManagerRef PM);

/** See llvm37::createFunctionInliningPass function. */
void LLVM37AddFunctionInliningPass(LLVM37PassManagerRef PM);

/** See llvm37::createAlwaysInlinerPass function. */
void LLVM37AddAlwaysInlinerPass(LLVM37PassManagerRef PM);

/** See llvm37::createGlobalDCEPass function. */
void LLVM37AddGlobalDCEPass(LLVM37PassManagerRef PM);

/** See llvm37::createGlobalOptimizerPass function. */
void LLVM37AddGlobalOptimizerPass(LLVM37PassManagerRef PM);

/** See llvm37::createIPConstantPropagationPass function. */
void LLVM37AddIPConstantPropagationPass(LLVM37PassManagerRef PM);

/** See llvm37::createPruneEHPass function. */
void LLVM37AddPruneEHPass(LLVM37PassManagerRef PM);

/** See llvm37::createIPSCCPPass function. */
void LLVM37AddIPSCCPPass(LLVM37PassManagerRef PM);

/** See llvm37::createInternalizePass function. */
void LLVM37AddInternalizePass(LLVM37PassManagerRef, unsigned AllButMain);

/** See llvm37::createStripDeadPrototypesPass function. */
void LLVM37AddStripDeadPrototypesPass(LLVM37PassManagerRef PM);

/** See llvm37::createStripSymbolsPass function. */
void LLVM37AddStripSymbolsPass(LLVM37PassManagerRef PM);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif
