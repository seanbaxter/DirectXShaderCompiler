/*===-- llvm37-c/Transform/PassManagerBuilder.h - PMB C Interface ---*- C -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to the PassManagerBuilder class.      *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_TRANSFORMS_PASSMANAGERBUILDER_H
#define LLVM37_C_TRANSFORMS_PASSMANAGERBUILDER_H

#include "llvm37-c/Core.h"

typedef struct LLVM37OpaquePassManagerBuilder *LLVM37PassManagerBuilderRef;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CTransformsPassManagerBuilder Pass manager builder
 * @ingroup LLVM37CTransforms
 *
 * @{
 */

/** See llvm37::PassManagerBuilder. */
LLVM37PassManagerBuilderRef LLVM37PassManagerBuilderCreate(void);
void LLVM37PassManagerBuilderDispose(LLVM37PassManagerBuilderRef PMB);

/** See llvm37::PassManagerBuilder::OptLevel. */
void
LLVM37PassManagerBuilderSetOptLevel(LLVM37PassManagerBuilderRef PMB,
                                  unsigned OptLevel);

/** See llvm37::PassManagerBuilder::SizeLevel. */
void
LLVM37PassManagerBuilderSetSizeLevel(LLVM37PassManagerBuilderRef PMB,
                                   unsigned SizeLevel);

/** See llvm37::PassManagerBuilder::DisableUnitAtATime. */
void
LLVM37PassManagerBuilderSetDisableUnitAtATime(LLVM37PassManagerBuilderRef PMB,
                                            LLVM37Bool Value);

/** See llvm37::PassManagerBuilder::DisableUnrollLoops. */
void
LLVM37PassManagerBuilderSetDisableUnrollLoops(LLVM37PassManagerBuilderRef PMB,
                                            LLVM37Bool Value);

/** See llvm37::PassManagerBuilder::DisableSimplifyLibCalls */
void
LLVM37PassManagerBuilderSetDisableSimplifyLibCalls(LLVM37PassManagerBuilderRef PMB,
                                                 LLVM37Bool Value);

/** See llvm37::PassManagerBuilder::Inliner. */
void
LLVM37PassManagerBuilderUseInlinerWithThreshold(LLVM37PassManagerBuilderRef PMB,
                                              unsigned Threshold);

/** See llvm37::PassManagerBuilder::populateFunctionPassManager. */
void
LLVM37PassManagerBuilderPopulateFunctionPassManager(LLVM37PassManagerBuilderRef PMB,
                                                  LLVM37PassManagerRef PM);

/** See llvm37::PassManagerBuilder::populateModulePassManager. */
void
LLVM37PassManagerBuilderPopulateModulePassManager(LLVM37PassManagerBuilderRef PMB,
                                                LLVM37PassManagerRef PM);

/** See llvm37::PassManagerBuilder::populateLTOPassManager. */
void LLVM37PassManagerBuilderPopulateLTOPassManager(LLVM37PassManagerBuilderRef PMB,
                                                  LLVM37PassManagerRef PM,
                                                  LLVM37Bool Internalize,
                                                  LLVM37Bool RunInliner);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
