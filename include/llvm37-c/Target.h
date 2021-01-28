/*===-- llvm37-c/Target.h - Target Lib C Iface --------------------*- C++ -*-===*/
/*                                                                            */
/*                     The LLVM37 Compiler Infrastructure                       */
/*                                                                            */
/* This file is distributed under the University of Illinois Open Source      */
/* License. See LICENSE.TXT for details.                                      */
/*                                                                            */
/*===----------------------------------------------------------------------===*/
/*                                                                            */
/* This header declares the C interface to libLLVM37Target.a, which             */
/* implements target information.                                             */
/*                                                                            */
/* Many exotic languages can interoperate with C code but have a harder time  */
/* with C++ due to name mangling. So in addition to C, this interface enables */
/* tools written in such languages.                                           */
/*                                                                            */
/*===----------------------------------------------------------------------===*/
  
#ifndef LLVM37_C_TARGET_H
#define LLVM37_C_TARGET_H

#include "llvm37-c/Core.h"
#include "llvm37/Config/llvm-config.h"

#if defined(_MSC_VER) && !defined(inline)
#define inline __inline
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup LLVM37CTarget Target information
 * @ingroup LLVM37C
 *
 * @{
 */

enum LLVM37ByteOrdering { LLVM37BigEndian, LLVM37LittleEndian };

typedef struct LLVM37OpaqueTargetData *LLVM37TargetDataRef;
typedef struct LLVM37OpaqueTargetLibraryInfotData *LLVM37TargetLibraryInfoRef;

/* Declare all of the target-initialization functions that are available. */
#define LLVM37_TARGET(TargetName) \
  void LLVM37Initialize##TargetName##TargetInfo(void);
#include "llvm37/Config/Targets.def"
#undef LLVM37_TARGET  /* Explicit undef to make SWIG happier */

#define LLVM37_TARGET(TargetName) void LLVM37Initialize##TargetName##Target(void);
#include "llvm37/Config/Targets.def"
#undef LLVM37_TARGET  /* Explicit undef to make SWIG happier */

#define LLVM37_TARGET(TargetName) \
  void LLVM37Initialize##TargetName##TargetMC(void);
#include "llvm37/Config/Targets.def"
#undef LLVM37_TARGET  /* Explicit undef to make SWIG happier */

/* Declare all of the available assembly printer initialization functions. */
#define LLVM37_ASM_PRINTER(TargetName) \
  void LLVM37Initialize##TargetName##AsmPrinter(void);
#include "llvm37/Config/AsmPrinters.def"
#undef LLVM37_ASM_PRINTER  /* Explicit undef to make SWIG happier */

/* Declare all of the available assembly parser initialization functions. */
#define LLVM37_ASM_PARSER(TargetName) \
  void LLVM37Initialize##TargetName##AsmParser(void);
#include "llvm37/Config/AsmParsers.def"
#undef LLVM37_ASM_PARSER  /* Explicit undef to make SWIG happier */

/* Declare all of the available disassembler initialization functions. */
#define LLVM37_DISASSEMBLER(TargetName) \
  void LLVM37Initialize##TargetName##Disassembler(void);
#include "llvm37/Config/Disassemblers.def"
#undef LLVM37_DISASSEMBLER  /* Explicit undef to make SWIG happier */

/** LLVM37InitializeAllTargetInfos - The main program should call this function if
    it wants access to all available targets that LLVM37 is configured to
    support. */
static inline void LLVM37InitializeAllTargetInfos(void) {
#define LLVM37_TARGET(TargetName) LLVM37Initialize##TargetName##TargetInfo();
#include "llvm37/Config/Targets.def"
#undef LLVM37_TARGET  /* Explicit undef to make SWIG happier */
}

/** LLVM37InitializeAllTargets - The main program should call this function if it
    wants to link in all available targets that LLVM37 is configured to
    support. */
static inline void LLVM37InitializeAllTargets(void) {
#define LLVM37_TARGET(TargetName) LLVM37Initialize##TargetName##Target();
#include "llvm37/Config/Targets.def"
#undef LLVM37_TARGET  /* Explicit undef to make SWIG happier */
}

/** LLVM37InitializeAllTargetMCs - The main program should call this function if
    it wants access to all available target MC that LLVM37 is configured to
    support. */
static inline void LLVM37InitializeAllTargetMCs(void) {
#define LLVM37_TARGET(TargetName) LLVM37Initialize##TargetName##TargetMC();
#include "llvm37/Config/Targets.def"
#undef LLVM37_TARGET  /* Explicit undef to make SWIG happier */
}

/** LLVM37InitializeAllAsmPrinters - The main program should call this function if
    it wants all asm printers that LLVM37 is configured to support, to make them
    available via the TargetRegistry. */
static inline void LLVM37InitializeAllAsmPrinters(void) {
#define LLVM37_ASM_PRINTER(TargetName) LLVM37Initialize##TargetName##AsmPrinter();
#include "llvm37/Config/AsmPrinters.def"
#undef LLVM37_ASM_PRINTER  /* Explicit undef to make SWIG happier */
}

/** LLVM37InitializeAllAsmParsers - The main program should call this function if
    it wants all asm parsers that LLVM37 is configured to support, to make them
    available via the TargetRegistry. */
static inline void LLVM37InitializeAllAsmParsers(void) {
#define LLVM37_ASM_PARSER(TargetName) LLVM37Initialize##TargetName##AsmParser();
#include "llvm37/Config/AsmParsers.def"
#undef LLVM37_ASM_PARSER  /* Explicit undef to make SWIG happier */
}

/** LLVM37InitializeAllDisassemblers - The main program should call this function
    if it wants all disassemblers that LLVM37 is configured to support, to make
    them available via the TargetRegistry. */
static inline void LLVM37InitializeAllDisassemblers(void) {
#define LLVM37_DISASSEMBLER(TargetName) \
  LLVM37Initialize##TargetName##Disassembler();
#include "llvm37/Config/Disassemblers.def"
#undef LLVM37_DISASSEMBLER  /* Explicit undef to make SWIG happier */
}

/** LLVM37InitializeNativeTarget - The main program should call this function to
    initialize the native target corresponding to the host.  This is useful
    for JIT applications to ensure that the target gets linked in correctly. */
static inline LLVM37Bool LLVM37InitializeNativeTarget(void) {
  /* If we have a native target, initialize it to ensure it is linked in. */
#ifdef LLVM37_NATIVE_TARGET
  LLVM37_NATIVE_TARGETINFO();
  LLVM37_NATIVE_TARGET();
  LLVM37_NATIVE_TARGETMC();
  return 0;
#else
  return 1;
#endif
}

/** LLVM37InitializeNativeTargetAsmParser - The main program should call this
    function to initialize the parser for the native target corresponding to the
    host. */
static inline LLVM37Bool LLVM37InitializeNativeAsmParser(void) {
#ifdef LLVM37_NATIVE_ASMPARSER
  LLVM37_NATIVE_ASMPARSER();
  return 0;
#else
  return 1;
#endif
}

/** LLVM37InitializeNativeTargetAsmPrinter - The main program should call this
    function to initialize the printer for the native target corresponding to
    the host. */
static inline LLVM37Bool LLVM37InitializeNativeAsmPrinter(void) {
#ifdef LLVM37_NATIVE_ASMPRINTER
  LLVM37_NATIVE_ASMPRINTER();
  return 0;
#else
  return 1;
#endif
}

/** LLVM37InitializeNativeTargetDisassembler - The main program should call this
    function to initialize the disassembler for the native target corresponding
    to the host. */
static inline LLVM37Bool LLVM37InitializeNativeDisassembler(void) {
#ifdef LLVM37_NATIVE_DISASSEMBLER
  LLVM37_NATIVE_DISASSEMBLER();
  return 0;
#else
  return 1;
#endif
}

/*===-- Target Data -------------------------------------------------------===*/

/** Creates target data from a target layout string.
    See the constructor llvm37::DataLayout::DataLayout. */
LLVM37TargetDataRef LLVM37CreateTargetData(const char *StringRep);

/** Adds target data information to a pass manager. This does not take ownership
    of the target data.
    See the method llvm37::PassManagerBase::add. */
void LLVM37AddTargetData(LLVM37TargetDataRef TD, LLVM37PassManagerRef PM);

/** Adds target library information to a pass manager. This does not take
    ownership of the target library info.
    See the method llvm37::PassManagerBase::add. */
void LLVM37AddTargetLibraryInfo(LLVM37TargetLibraryInfoRef TLI,
                              LLVM37PassManagerRef PM);

/** Converts target data to a target layout string. The string must be disposed
    with LLVM37DisposeMessage.
    See the constructor llvm37::DataLayout::DataLayout. */
char *LLVM37CopyStringRepOfTargetData(LLVM37TargetDataRef TD);

/** Returns the byte order of a target, either LLVM37BigEndian or
    LLVM37LittleEndian.
    See the method llvm37::DataLayout::isLittleEndian. */
enum LLVM37ByteOrdering LLVM37ByteOrder(LLVM37TargetDataRef TD);

/** Returns the pointer size in bytes for a target.
    See the method llvm37::DataLayout::getPointerSize. */
unsigned LLVM37PointerSize(LLVM37TargetDataRef TD);

/** Returns the pointer size in bytes for a target for a specified
    address space.
    See the method llvm37::DataLayout::getPointerSize. */
unsigned LLVM37PointerSizeForAS(LLVM37TargetDataRef TD, unsigned AS);

/** Returns the integer type that is the same size as a pointer on a target.
    See the method llvm37::DataLayout::getIntPtrType. */
LLVM37TypeRef LLVM37IntPtrType(LLVM37TargetDataRef TD);

/** Returns the integer type that is the same size as a pointer on a target.
    This version allows the address space to be specified.
    See the method llvm37::DataLayout::getIntPtrType. */
LLVM37TypeRef LLVM37IntPtrTypeForAS(LLVM37TargetDataRef TD, unsigned AS);

/** Returns the integer type that is the same size as a pointer on a target.
    See the method llvm37::DataLayout::getIntPtrType. */
LLVM37TypeRef LLVM37IntPtrTypeInContext(LLVM37ContextRef C, LLVM37TargetDataRef TD);

/** Returns the integer type that is the same size as a pointer on a target.
    This version allows the address space to be specified.
    See the method llvm37::DataLayout::getIntPtrType. */
LLVM37TypeRef LLVM37IntPtrTypeForASInContext(LLVM37ContextRef C, LLVM37TargetDataRef TD,
                                         unsigned AS);

/** Computes the size of a type in bytes for a target.
    See the method llvm37::DataLayout::getTypeSizeInBits. */
unsigned long long LLVM37SizeOfTypeInBits(LLVM37TargetDataRef TD, LLVM37TypeRef Ty);

/** Computes the storage size of a type in bytes for a target.
    See the method llvm37::DataLayout::getTypeStoreSize. */
unsigned long long LLVM37StoreSizeOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty);

/** Computes the ABI size of a type in bytes for a target.
    See the method llvm37::DataLayout::getTypeAllocSize. */
unsigned long long LLVM37ABISizeOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty);

/** Computes the ABI alignment of a type in bytes for a target.
    See the method llvm37::DataLayout::getTypeABISize. */
unsigned LLVM37ABIAlignmentOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty);

/** Computes the call frame alignment of a type in bytes for a target.
    See the method llvm37::DataLayout::getTypeABISize. */
unsigned LLVM37CallFrameAlignmentOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty);

/** Computes the preferred alignment of a type in bytes for a target.
    See the method llvm37::DataLayout::getTypeABISize. */
unsigned LLVM37PreferredAlignmentOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty);

/** Computes the preferred alignment of a global variable in bytes for a target.
    See the method llvm37::DataLayout::getPreferredAlignment. */
unsigned LLVM37PreferredAlignmentOfGlobal(LLVM37TargetDataRef TD,
                                        LLVM37ValueRef GlobalVar);

/** Computes the structure element that contains the byte offset for a target.
    See the method llvm37::StructLayout::getElementContainingOffset. */
unsigned LLVM37ElementAtOffset(LLVM37TargetDataRef TD, LLVM37TypeRef StructTy,
                             unsigned long long Offset);

/** Computes the byte offset of the indexed struct element for a target.
    See the method llvm37::StructLayout::getElementContainingOffset. */
unsigned long long LLVM37OffsetOfElement(LLVM37TargetDataRef TD,
                                       LLVM37TypeRef StructTy, unsigned Element);

/** Deallocates a TargetData.
    See the destructor llvm37::DataLayout::~DataLayout. */
void LLVM37DisposeTargetData(LLVM37TargetDataRef TD);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* defined(__cplusplus) */

#endif

