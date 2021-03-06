//===---- llvm37/IRReader/IRReader.h - Reader for LLVM37 IR files ---*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines functions for reading LLVM37 IR. They support both
// Bitcode and Assembly, automatically detecting the input format.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_IRREADER_IRREADER_H
#define LLVM37_IRREADER_IRREADER_H

#include "llvm37/Support/MemoryBuffer.h"
#include <string>

namespace llvm37 {

class Module;
class SMDiagnostic;
class LLVM37Context;

/// If the given file holds a bitcode image, return a Module
/// for it which does lazy deserialization of function bodies.  Otherwise,
/// attempt to parse it as LLVM37 Assembly and return a fully populated
/// Module.
std::unique_ptr<Module> getLazyIRFileModule(StringRef Filename,
                                            SMDiagnostic &Err,
                                            LLVM37Context &Context);

/// If the given MemoryBuffer holds a bitcode image, return a Module
/// for it.  Otherwise, attempt to parse it as LLVM37 Assembly and return
/// a Module for it.
std::unique_ptr<Module> parseIR(MemoryBufferRef Buffer, SMDiagnostic &Err,
                                LLVM37Context &Context);

/// If the given file holds a bitcode image, return a Module for it.
/// Otherwise, attempt to parse it as LLVM37 Assembly and return a Module
/// for it.
std::unique_ptr<Module> parseIRFile(StringRef Filename, SMDiagnostic &Err,
                                    LLVM37Context &Context);
}

#endif
