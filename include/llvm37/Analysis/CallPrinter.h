//===-- CallPrinter.h - Call graph printer external interface ----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines external functions that can be called to explicitly
// instantiate the call graph printer.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_ANALYSIS_CALLPRINTER_H
#define LLVM37_ANALYSIS_CALLPRINTER_H

namespace llvm37 {

  class ModulePass;

  ModulePass *createCallGraphViewerPass();
  ModulePass *createCallGraphPrinterPass();

} // end namespace llvm37

#endif
