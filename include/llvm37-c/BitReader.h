/*===-- llvm37-c/BitReader.h - BitReader Library C Interface ------*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37BitReader.a, which          *|
|* implements input of the LLVM37 bitcode format.                               *|
|*                                                                            *|
|* Many exotic languages can interoperate with C code but have a harder time  *|
|* with C++ due to name mangling. So in addition to C, this interface enables *|
|* tools written in such languages.                                           *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_BITREADER_H
#define LLVM37_C_BITREADER_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CBitReader Bit Reader
 * @ingroup LLVM37C
 *
 * @{
 */

/* Builds a module from the bitcode in the specified memory buffer, returning a
   reference to the module via the OutModule parameter. Returns 0 on success.
   Optionally returns a human-readable error message via OutMessage. */
LLVM37Bool LLVM37ParseBitcode(LLVM37MemoryBufferRef MemBuf,
                          LLVM37ModuleRef *OutModule, char **OutMessage);

LLVM37Bool LLVM37ParseBitcodeInContext(LLVM37ContextRef ContextRef,
                                   LLVM37MemoryBufferRef MemBuf,
                                   LLVM37ModuleRef *OutModule, char **OutMessage);

/** Reads a module from the specified path, returning via the OutMP parameter
    a module provider which performs lazy deserialization. Returns 0 on success.
    Optionally returns a human-readable error message via OutMessage. */
LLVM37Bool LLVM37GetBitcodeModuleInContext(LLVM37ContextRef ContextRef,
                                       LLVM37MemoryBufferRef MemBuf,
                                       LLVM37ModuleRef *OutM,
                                       char **OutMessage);

LLVM37Bool LLVM37GetBitcodeModule(LLVM37MemoryBufferRef MemBuf, LLVM37ModuleRef *OutM,
                              char **OutMessage);


/** Deprecated: Use LLVM37GetBitcodeModuleInContext instead. */
LLVM37Bool LLVM37GetBitcodeModuleProviderInContext(LLVM37ContextRef ContextRef,
                                               LLVM37MemoryBufferRef MemBuf,
                                               LLVM37ModuleProviderRef *OutMP,
                                               char **OutMessage);

/** Deprecated: Use LLVM37GetBitcodeModule instead. */
LLVM37Bool LLVM37GetBitcodeModuleProvider(LLVM37MemoryBufferRef MemBuf,
                                      LLVM37ModuleProviderRef *OutMP,
                                      char **OutMessage);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
