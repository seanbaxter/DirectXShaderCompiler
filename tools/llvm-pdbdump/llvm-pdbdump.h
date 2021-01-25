//===- llvm-pdbdump.h ----------------------------------------- *- C++ --*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVMPDBDUMP_LLVMPDBDUMP_H
#define LLVM37_TOOLS_LLVMPDBDUMP_LLVMPDBDUMP_H

#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/raw_ostream.h"

namespace opts {
extern llvm37::cl::opt<bool> Compilands;
extern llvm37::cl::opt<bool> Symbols;
extern llvm37::cl::opt<bool> Globals;
extern llvm37::cl::opt<bool> Types;
extern llvm37::cl::opt<bool> All;

extern llvm37::cl::opt<bool> ExcludeCompilerGenerated;

extern llvm37::cl::opt<bool> NoClassDefs;
extern llvm37::cl::opt<bool> NoEnumDefs;
extern llvm37::cl::list<std::string> ExcludeTypes;
extern llvm37::cl::list<std::string> ExcludeSymbols;
extern llvm37::cl::list<std::string> ExcludeCompilands;
}

#endif