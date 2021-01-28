/*===---------------------------Vectorize.h --------------------- -*- C -*-===*\
|*===----------- Vectorization Transformation Library C Interface ---------===*|
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37Vectorize.a, which          *|
|* implements various vectorization transformations of the LLVM37 IR.           *|
|*                                                                            *|
|* Many exotic languages can interoperate with C code but have a harder time  *|
|* with C++ due to name mangling. So in addition to C, this interface enables *|
|* tools written in such languages.                                           *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_TRANSFORMS_VECTORIZE_H
#define LLVM37_C_TRANSFORMS_VECTORIZE_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CTransformsVectorize Vectorization transformations
 * @ingroup LLVM37CTransforms
 *
 * @{
 */

/** See llvm37::createBBVectorizePass function. */
void LLVM37AddBBVectorizePass(LLVM37PassManagerRef PM);

/** See llvm37::createLoopVectorizePass function. */
void LLVM37AddLoopVectorizePass(LLVM37PassManagerRef PM);

/** See llvm37::createSLPVectorizerPass function. */
void LLVM37AddSLPVectorizePass(LLVM37PassManagerRef PM);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif

