//===- CallPrinter.cpp - DOT printer for call graph -----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines '-dot-callgraph', which emit a callgraph.<fnname>.dot
// containing the call graph of a module.
//
// There is also a pass available to directly call dotty ('-view-callgraph').
//
//===----------------------------------------------------------------------===//

#include "llvm37/Analysis/CallGraph.h"
#include "llvm37/Analysis/CallPrinter.h"
#include "llvm37/Analysis/DOTGraphTraitsPass.h"

using namespace llvm37;

namespace llvm37 {

template <> struct DOTGraphTraits<CallGraph *> : public DefaultDOTGraphTraits {
  DOTGraphTraits(bool isSimple = false) : DefaultDOTGraphTraits(isSimple) {}

  static std::string getGraphName(CallGraph *Graph) { return "Call graph"; }

  std::string getNodeLabel(CallGraphNode *Node, CallGraph *Graph) {
    if (Function *Func = Node->getFunction())
      return Func->getName();

    return "external node";
  }
};

struct AnalysisCallGraphWrapperPassTraits {
  static CallGraph *getGraph(CallGraphWrapperPass *P) {
    return &P->getCallGraph();
  }
};

} // end llvm37 namespace

namespace {

struct CallGraphViewer
    : public DOTGraphTraitsModuleViewer<CallGraphWrapperPass, true, CallGraph *,
                                        AnalysisCallGraphWrapperPassTraits> {
  static char ID;

  CallGraphViewer()
      : DOTGraphTraitsModuleViewer<CallGraphWrapperPass, true, CallGraph *,
                                   AnalysisCallGraphWrapperPassTraits>(
            "callgraph", ID) {
    initializeCallGraphViewerPass(*PassRegistry::getPassRegistry());
  }
};

struct CallGraphPrinter : public DOTGraphTraitsModulePrinter<
                              CallGraphWrapperPass, true, CallGraph *,
                              AnalysisCallGraphWrapperPassTraits> {
  static char ID;

  CallGraphPrinter()
      : DOTGraphTraitsModulePrinter<CallGraphWrapperPass, true, CallGraph *,
                                    AnalysisCallGraphWrapperPassTraits>(
            "callgraph", ID) {
    initializeCallGraphPrinterPass(*PassRegistry::getPassRegistry());
  }
};

} // end anonymous namespace

char CallGraphViewer::ID = 0;
INITIALIZELLVM37_PASS(CallGraphViewer, "view-callgraph", "View call graph", false,
                false)

char CallGraphPrinter::ID = 0;
INITIALIZELLVM37_PASS(CallGraphPrinter, "dot-callgraph",
                "Print call graph to 'dot' file", false, false)

// Create methods available outside of this file, to use them
// "include/llvm37/LinkAllPasses.h". Otherwise the pass would be deleted by
// the link time optimization.

ModulePass *llvm37::createCallGraphViewerPass() { return new CallGraphViewer(); }

ModulePass *llvm37::createCallGraphPrinterPass() {
  return new CallGraphPrinter();
}
