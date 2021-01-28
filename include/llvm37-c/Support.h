/*===-- llvm37-c/Support.h - Support C Interface --------------------*- C -*-===*\
|*                                                                            *|
|*                     The LLVM37 Compiler Infrastructure                       *|
|*                                                                            *|
|* This file is distributed under the University of Illinois Open Source      *|
|* License. See LICENSE.TXT for details.                                      *|
|*                                                                            *|
|*===----------------------------------------------------------------------===*|
|*                                                                            *|
|* This file defines the C interface to the LLVM37 support library.             *|
|*                                                                            *|
\*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_SUPPORT_H
#define LLVM37_C_SUPPORT_H

#include "llvm37/Support/DataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CSupportTypes Types and Enumerations
 *
 * @{
 */

typedef int LLVM37Bool;

/**
 * Used to pass regions of memory through LLVM37 interfaces.
 *
 * @see llvm37::MemoryBuffer
 */
typedef struct LLVM37OpaqueMemoryBuffer *LLVM37MemoryBufferRef;

/**
 * @}
 */

/**
 * This function permanently loads the dynamic library at the given path.
 * It is safe to call this function multiple times for the same library.
 *
 * @see sys::DynamicLibrary::LoadLibraryPermanently()
  */
LLVM37Bool LLVM37LoadLibraryPermanently(const char* Filename);

/**
 * This function parses the given arguments using the LLVM37 command line parser.
 * Note that the only stable thing about this function is its signature; you
 * cannot rely on any particular set of command line arguments being interpreted
 * the same way across LLVM37 versions.
 *
 * @see llvm37::cl::ParseCommandLineOptions()
 */
void LLVM37ParseCommandLineOptions(int argc, const char *const *argv,
                                 const char *Overview);

/**
 * This function will search through all previously loaded dynamic
 * libraries for the symbol \p symbolName. If it is found, the address of
 * that symbol is returned. If not, null is returned.
 *
 * @see sys::DynamicLibrary::SearchForAddressOfSymbol()
 */
void *LLVM37SearchForAddressOfSymbol(const char *symbolName);

/**
 * This functions permanently adds the symbol \p symbolName with the
 * value \p symbolValue.  These symbols are searched before any
 * libraries.
 *
 * @see sys::DynamicLibrary::AddSymbol()
 */
void LLVM37AddSymbol(const char *symbolName, void *symbolValue);

#ifdef __cplusplus
}
#endif

#endif
