/*===-- llvm37-c/BitWriter.h - BitWriter Library C Interface ------*- C++ -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This header declares the C interface to libLLVM37BitWriter.a, which          *|
|* implements output of the LLVM37 bitcode format.                              *|
|*                                                                            *|
|* Many exotic languages can interoperate with C code but have a harder time  *|
|* with C++ due to name mangling. So in addition to C, this interface enables *|
|* tools written in such languages.                                           *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_BITWRITER_H
#define LLVM37_C_BITWRITER_H

#include "llvm37-c/Core.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CBitWriter Bit Writer
 * @ingroup LLVM37C
 *
 * @{
 */

/*===-- Operations on modules ---------------------------------------------===*/

/** Writes a module to the specified path. Returns 0 on success. */
int LLVM37WriteBitcodeToFile(LLVM37ModuleRef M, const char *Path);

/** Writes a module to an open file descriptor. Returns 0 on success. */
int LLVM37WriteBitcodeToFD(LLVM37ModuleRef M, int FD, int ShouldClose,
                         int Unbuffered);

/** Deprecated for LLVM37WriteBitcodeToFD. Writes a module to an open file
    descriptor. Returns 0 on success. Closes the Handle. */
int LLVM37WriteBitcodeToFileHandle(LLVM37ModuleRef M, int Handle);

/** Writes a module to a new memory buffer and returns it. */
LLVM37MemoryBufferRef LLVM37WriteBitcodeToMemoryBuffer(LLVM37ModuleRef M);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif
