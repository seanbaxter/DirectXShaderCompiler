//===- GraphPrinters.cpp - DOT printers for various graph types -----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines several printers for various different types of graphs used
// by the LLVM37 infrastructure.  It uses the generic graph interface to convert
// the graph into a .dot graph.  These graphs can then be processed with the
// "dot" tool to convert them to postscript or some other suitable format.
//
//===----------------------------------------------------------------------===//

#include "llvm37/IR/Dominators.h"
#include "llvm37/Pass.h"

using namespace llvm37;

//===----------------------------------------------------------------------===//
//                            DomInfoPrinter Pass
//===----------------------------------------------------------------------===//

namespace {
  class DomInfoPrinter : public FunctionPass {
  public:
    static char ID; // Pass identification, replacement for typeid
    DomInfoPrinter() : FunctionPass(ID) {}

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      AU.setPreservesAll();
      AU.addRequired<DominatorTreeWrapperPass>();
    }

    bool runOnFunction(Function &F) override {
      getAnalysis<DominatorTreeWrapperPass>().dump();
      return false;
    }
  };
}

char DomInfoPrinter::ID = 0;
static RegisterPass<DomInfoPrinter>
DIP("print-dom-info", "Dominator Info Printer", true, true);
