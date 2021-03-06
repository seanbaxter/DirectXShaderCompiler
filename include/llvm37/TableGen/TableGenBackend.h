//===- llvm37/TableGen/TableGenBackend.h - Backend utilities ------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Useful utilities for TableGen backends.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TABLEGEN_TABLEGENBACKEND_H
#define LLVM37_TABLEGEN_TABLEGENBACKEND_H

namespace llvm37 {

class StringRef;
class raw_ostream;

/// emitSourceFileHeader - Output an LLVM37 style file header to the specified
/// raw_ostream.
void emitSourceFileHeader(StringRef Desc, raw_ostream &OS);

} // End llvm37 namespace

#endif
