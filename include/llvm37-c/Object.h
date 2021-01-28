/*===-- llvm37-c/Object.h - Object Lib C Iface --------------------*- C++ -*-===*/
/*                                                                            */
/*                     The LLVM37 Compiler Infrastructure                       */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/
/*                                                                            */
/* This header declares the C interface to libLLVM37Object.a, which             */
/* implements object file reading and writing.                                */
/*                                                                            */
/* Many exotic languages can interoperate with C code but have a harder time  */
/* with C++ due to name mangling. So in addition to C, this interface enables */
/* tools written in such languages.                                           */
/*                                                                            */
/*===----------------------------------------------------------------------===*/

#ifndef LLVM37_C_OBJECT_H
#define LLVM37_C_OBJECT_H

#include "llvm37-c/Core.h"
#include "llvm37/Config/llvm-config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CObject Object file reading and writing
 * @ingroup LLVM37C
 *
 * @{
 */

// Opaque type wrappers
typedef struct LLVM37OpaqueObjectFile *LLVM37ObjectFileRef;
typedef struct LLVM37OpaqueSectionIterator *LLVM37SectionIteratorRef;
typedef struct LLVM37OpaqueSymbolIterator *LLVM37SymbolIteratorRef;
typedef struct LLVM37OpaqueRelocationIterator *LLVM37RelocationIteratorRef;

// ObjectFile creation
LLVM37ObjectFileRef LLVM37CreateObjectFile(LLVM37MemoryBufferRef MemBuf);
void LLVM37DisposeObjectFile(LLVM37ObjectFileRef ObjectFile);

// ObjectFile Section iterators
LLVM37SectionIteratorRef LLVM37GetSections(LLVM37ObjectFileRef ObjectFile);
void LLVM37DisposeSectionIterator(LLVM37SectionIteratorRef SI);
LLVM37Bool LLVM37IsSectionIteratorAtEnd(LLVM37ObjectFileRef ObjectFile,
                                LLVM37SectionIteratorRef SI);
void LLVM37MoveToNextSection(LLVM37SectionIteratorRef SI);
void LLVM37MoveToContainingSection(LLVM37SectionIteratorRef Sect,
                                 LLVM37SymbolIteratorRef Sym);

// ObjectFile Symbol iterators
LLVM37SymbolIteratorRef LLVM37GetSymbols(LLVM37ObjectFileRef ObjectFile);
void LLVM37DisposeSymbolIterator(LLVM37SymbolIteratorRef SI);
LLVM37Bool LLVM37IsSymbolIteratorAtEnd(LLVM37ObjectFileRef ObjectFile,
                                LLVM37SymbolIteratorRef SI);
void LLVM37MoveToNextSymbol(LLVM37SymbolIteratorRef SI);

// SectionRef accessors
const char *LLVM37GetSectionName(LLVM37SectionIteratorRef SI);
uint64_t LLVM37GetSectionSize(LLVM37SectionIteratorRef SI);
const char *LLVM37GetSectionContents(LLVM37SectionIteratorRef SI);
uint64_t LLVM37GetSectionAddress(LLVM37SectionIteratorRef SI);
LLVM37Bool LLVM37GetSectionContainsSymbol(LLVM37SectionIteratorRef SI,
                                 LLVM37SymbolIteratorRef Sym);

// Section Relocation iterators
LLVM37RelocationIteratorRef LLVM37GetRelocations(LLVM37SectionIteratorRef Section);
void LLVM37DisposeRelocationIterator(LLVM37RelocationIteratorRef RI);
LLVM37Bool LLVM37IsRelocationIteratorAtEnd(LLVM37SectionIteratorRef Section,
                                       LLVM37RelocationIteratorRef RI);
void LLVM37MoveToNextRelocation(LLVM37RelocationIteratorRef RI);


// SymbolRef accessors
const char *LLVM37GetSymbolName(LLVM37SymbolIteratorRef SI);
uint64_t LLVM37GetSymbolAddress(LLVM37SymbolIteratorRef SI);
uint64_t LLVM37GetSymbolSize(LLVM37SymbolIteratorRef SI);

// RelocationRef accessors
uint64_t LLVM37GetRelocationOffset(LLVM37RelocationIteratorRef RI);
LLVM37SymbolIteratorRef LLVM37GetRelocationSymbol(LLVM37RelocationIteratorRef RI);
uint64_t LLVM37GetRelocationType(LLVM37RelocationIteratorRef RI);
// NOTE: Caller takes ownership of returned string of the two
// following functions.
const char *LLVM37GetRelocationTypeName(LLVM37RelocationIteratorRef RI);
const char *LLVM37GetRelocationValueString(LLVM37RelocationIteratorRef RI);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif

