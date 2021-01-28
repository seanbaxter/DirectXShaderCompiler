/*===-- llvm37-c/Analysis.h - Analysis Library C Interface --------*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37Analysis.a, which           *|
|* implements various analyses of the LLVM37 IR.                                *|
|*                                                                            *|
|* Many exotic languages can interoperate with C code but have a harder time  *|
|* with C++ due to name mangling. So in addition to C, this interface enables *|
|* tools written in such languages.                                           *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_ANALYSIS_H
#define LLVM37_C_ANALYSIS_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CAnalysis Analysis
 * @ingroup LLVM37C
 *
 * @{
 */

typedef enum {
  LLVM37AbortProcessAction, /* verifier will print to stderr and abort() */
  LLVM37PrintMessageAction, /* verifier will print to stderr and return 1 */
  LLVM37ReturnStatusAction  /* verifier will just return 1 */
} LLVM37VerifierFailureAction;


/* Verifies that a module is valid, taking the specified action if not.
   Optionally returns a human-readable description of any invalid constructs.
   OutMessage must be disposed with LLVM37DisposeMessage. */
LLVM37Bool LLVM37VerifyModule(LLVM37ModuleRef M, LLVM37VerifierFailureAction Action,
                          _Outptr_opt_ char **OutMessage);

/* Verifies that a single function is valid, taking the specified action. Useful
   for debugging. */
LLVM37Bool LLVM37VerifyFunction(LLVM37ValueRef Fn, LLVM37VerifierFailureAction Action);

/* Open up a ghostview window that displays the CFG of the current function.
   Useful for debugging. */
void LLVM37ViewFunctionCFG(LLVM37ValueRef Fn);
void LLVM37ViewFunctionCFGOnly(LLVM37ValueRef Fn);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
