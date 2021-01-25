//===-- llvm-readobj.h ----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37_READOBJ_LLVM37_READOBJ_H
#define LLVM37_TOOLS_LLVM37_READOBJ_LLVM37_READOBJ_H

#include "llvm37/Support/CommandLine.h"
#include <string>

namespace llvm37 {
  namespace object {
    class RelocationRef;
  }

  // Various helper functions.
  bool error(std::error_code ec);
  bool relocAddressLess(object::RelocationRef A,
                        object::RelocationRef B);
} // namespace llvm37

namespace opts {
  extern llvm37::cl::list<std::string> InputFilenames;
  extern llvm37::cl::opt<bool> FileHeaders;
  extern llvm37::cl::opt<bool> Sections;
  extern llvm37::cl::opt<bool> SectionRelocations;
  extern llvm37::cl::opt<bool> SectionSymbols;
  extern llvm37::cl::opt<bool> SectionData;
  extern llvm37::cl::opt<bool> Relocations;
  extern llvm37::cl::opt<bool> Symbols;
  extern llvm37::cl::opt<bool> DynamicSymbols;
  extern llvm37::cl::opt<bool> UnwindInfo;
  extern llvm37::cl::opt<bool> ExpandRelocs;
  extern llvm37::cl::opt<bool> CodeView;
  extern llvm37::cl::opt<bool> CodeViewSubsectionBytes;
  extern llvm37::cl::opt<bool> ARMAttributes;
  extern llvm37::cl::opt<bool> MipsPLTGOT;
} // namespace opts

#define LLVM37_READOBJ_ENUM_ENT(ns, enum) \
  { #enum, ns::enum }

#endif
