//===-- llvm-cxxdump.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37_CXXDUMP_LLVM37_CXXDUMP_H
#define LLVM37_TOOLS_LLVM37_CXXDUMP_LLVM37_CXXDUMP_H

#include "llvm37/Support/CommandLine.h"
#include <string>

namespace opts {
extern llvm37::cl::list<std::string> InputFilenames;
} // namespace opts

#define LLVM37_CXXDUMP_ENUM_ENT(ns, enum)                                        \
  { #enum, ns::enum }

#endif
