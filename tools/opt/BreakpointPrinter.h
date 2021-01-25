//===- BreakpointPrinter.h - Breakpoint location printer ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Breakpoint location printer.
///
//===----------------------------------------------------------------------===//
#ifndef LLVM37_TOOLS_OPT_BREAKPOINTPRINTER_H
#define LLVM37_TOOLS_OPT_BREAKPOINTPRINTER_H

namespace llvm37 {

class ModulePass;
class raw_ostream;

ModulePass *createBreakpointPrinter(raw_ostream &out);
}

#endif // LLVM37_TOOLS_OPT_BREAKPOINTPRINTER_H
