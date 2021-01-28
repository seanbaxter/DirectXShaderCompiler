/*===-- llvm37-c/Initialization.h - Initialization C Interface ------*- C -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to LLVM37 initialization routines,      *|
|* which must be called before you can use the functionality provided by      *|
|* the corresponding LLVM37 library.                                            *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_INITIALIZATION_H
#define LLVM37_C_INITIALIZATION_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CInitialization Initialization Routines
 * @ingroup LLVM37C
 *
 * This module contains routines used to initialize the LLVM37 system.
 *
 * @{
 */

void LLVM37InitializeCore(LLVM37PassRegistryRef R);
void LLVM37InitializeTransformUtils(LLVM37PassRegistryRef R);
void LLVM37InitializeScalarOpts(LLVM37PassRegistryRef R);
void LLVM37InitializeObjCARCOpts(LLVM37PassRegistryRef R);
void LLVM37InitializeVectorization(LLVM37PassRegistryRef R);
void LLVM37InitializeInstCombine(LLVM37PassRegistryRef R);
void LLVM37InitializeIPO(LLVM37PassRegistryRef R);
void LLVM37InitializeInstrumentation(LLVM37PassRegistryRef R);
void LLVM37InitializeAnalysis(LLVM37PassRegistryRef R);
void LLVM37InitializeIPA(LLVM37PassRegistryRef R);
void LLVM37InitializeCodeGen(LLVM37PassRegistryRef R);
void LLVM37InitializeTarget(LLVM37PassRegistryRef R);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
