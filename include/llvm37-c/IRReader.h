/*===-- llvm37-c/IRReader.h - IR Reader C Interface -----------------*- C -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file defines the C interface to the IR Reader.                        *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_IRREADER_H
#define LLVM37_C_IRREADER_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Read LLVM37 IR from a memory buffer and convert it into an in-memory Module
 * object. Returns 0 on success.
 * Optionally returns a human-readable description of any errors that
 * occurred during parsing IR. OutMessage must be disposed with
 * LLVM37DisposeMessage.
 *
 * @see llvm37::ParseIR()
 */
LLVM37Bool LLVM37ParseIRInContext(LLVM37ContextRef ContextRef,
                              LLVM37MemoryBufferRef MemBuf, LLVM37ModuleRef *OutM,
                              char **OutMessage);

#ifdef __cplusplus
}
#endif

#endif
