//===- TargetSelect.h - Target Selection & Registration ---------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides utilities to make sure that certain classes of targets are
// linked into the main application executable, and initialize them as
// appropriate.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_TARGETSELECT_H
#define LLVM37_SUPPORT_TARGETSELECT_H

#include "llvm37/Config/llvm-config.h"

extern "C" {
  // Declare all of the target-initialization functions that are available.
#define LLVM37_TARGET(TargetName) void LLVM37Initialize##TargetName##TargetInfo();
#include "llvm37/Config/Targets.def"

#define LLVM37_TARGET(TargetName) void LLVM37Initialize##TargetName##Target();
#include "llvm37/Config/Targets.def"
  
  // Declare all of the target-MC-initialization functions that are available.
#define LLVM37_TARGET(TargetName) void LLVM37Initialize##TargetName##TargetMC();
#include "llvm37/Config/Targets.def"
  
  // Declare all of the available assembly printer initialization functions.
#define LLVM37_ASM_PRINTER(TargetName) void LLVM37Initialize##TargetName##AsmPrinter();
#include "llvm37/Config/AsmPrinters.def"

  // Declare all of the available assembly parser initialization functions.
#define LLVM37_ASM_PARSER(TargetName) void LLVM37Initialize##TargetName##AsmParser();
#include "llvm37/Config/AsmParsers.def"

  // Declare all of the available disassembler initialization functions.
#define LLVM37_DISASSEMBLER(TargetName) \
  void LLVM37Initialize##TargetName##Disassembler();
#include "llvm37/Config/Disassemblers.def"
}

namespace llvm37 {
  /// InitializeAllTargetInfos - The main program should call this function if
  /// it wants access to all available targets that LLVM37 is configured to
  /// support, to make them available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllTargetInfos() {
#define LLVM37_TARGET(TargetName) LLVM37Initialize##TargetName##TargetInfo();
#include "llvm37/Config/Targets.def"
  }
  
  /// InitializeAllTargets - The main program should call this function if it
  /// wants access to all available target machines that LLVM37 is configured to
  /// support, to make them available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllTargets() {
    // FIXME: Remove this, clients should do it.
    InitializeAllTargetInfos();

#define LLVM37_TARGET(TargetName) LLVM37Initialize##TargetName##Target();
#include "llvm37/Config/Targets.def"
  }
  
  /// InitializeAllTargetMCs - The main program should call this function if it
  /// wants access to all available target MC that LLVM37 is configured to
  /// support, to make them available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllTargetMCs() {
#define LLVM37_TARGET(TargetName) LLVM37Initialize##TargetName##TargetMC();
#include "llvm37/Config/Targets.def"
  }
  
  /// InitializeAllAsmPrinters - The main program should call this function if
  /// it wants all asm printers that LLVM37 is configured to support, to make them
  /// available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllAsmPrinters() {
#define LLVM37_ASM_PRINTER(TargetName) LLVM37Initialize##TargetName##AsmPrinter();
#include "llvm37/Config/AsmPrinters.def"
  }
  
  /// InitializeAllAsmParsers - The main program should call this function if it
  /// wants all asm parsers that LLVM37 is configured to support, to make them
  /// available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllAsmParsers() {
#define LLVM37_ASM_PARSER(TargetName) LLVM37Initialize##TargetName##AsmParser();
#include "llvm37/Config/AsmParsers.def"
  }
  
  /// InitializeAllDisassemblers - The main program should call this function if
  /// it wants all disassemblers that LLVM37 is configured to support, to make
  /// them available via the TargetRegistry.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline void InitializeAllDisassemblers() {
#define LLVM37_DISASSEMBLER(TargetName) LLVM37Initialize##TargetName##Disassembler();
#include "llvm37/Config/Disassemblers.def"
  }
  
  /// InitializeNativeTarget - The main program should call this function to
  /// initialize the native target corresponding to the host.  This is useful 
  /// for JIT applications to ensure that the target gets linked in correctly.
  ///
  /// It is legal for a client to make multiple calls to this function.
  inline bool InitializeNativeTarget() {
  // If we have a native target, initialize it to ensure it is linked in.
#ifdef LLVM37_NATIVE_TARGET
    LLVM37_NATIVE_TARGETINFO();
    LLVM37_NATIVE_TARGET();
    LLVM37_NATIVE_TARGETMC();
    return false;
#else
    return true;
#endif
  }  

  /// InitializeNativeTargetAsmPrinter - The main program should call
  /// this function to initialize the native target asm printer.
  inline bool InitializeNativeTargetAsmPrinter() {
  // If we have a native target, initialize the corresponding asm printer.
#ifdef LLVM37_NATIVE_ASMPRINTER
    LLVM37_NATIVE_ASMPRINTER();
    return false;
#else
    return true;
#endif
  }  

  /// InitializeNativeTargetAsmParser - The main program should call
  /// this function to initialize the native target asm parser.
  inline bool InitializeNativeTargetAsmParser() {
  // If we have a native target, initialize the corresponding asm parser.
#ifdef LLVM37_NATIVE_ASMPARSER
    LLVM37_NATIVE_ASMPARSER();
    return false;
#else
    return true;
#endif
  }  

  /// InitializeNativeTargetDisassembler - The main program should call
  /// this function to initialize the native target disassembler.
  inline bool InitializeNativeTargetDisassembler() {
  // If we have a native target, initialize the corresponding disassembler.
#ifdef LLVM37_NATIVE_DISASSEMBLER
    LLVM37_NATIVE_DISASSEMBLER();
    return false;
#else
    return true;
#endif
  }  

}

#endif
