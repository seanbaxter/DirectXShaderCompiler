//===-------- EdgeBundles.cpp - Bundles of CFG edges ----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file provides the implementation of the EdgeBundles analysis.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/EdgeBundles.h"
#include "llvm37/CodeGen/MachineBasicBlock.h"
#include "llvm37/CodeGen/MachineFunction.h"
#include "llvm37/CodeGen/Passes.h"
#include "llvm37/Support/CommandLine.h"
#include "llvm37/Support/GraphWriter.h"

using namespace llvm37;

static cl::opt<bool>
ViewEdgeBundles("view-edge-bundles", cl::Hidden,
                cl::desc("Pop up a window to show edge bundle graphs"));

char EdgeBundles::ID = 0;

INITIALIZE_PASS(EdgeBundles, "edge-bundles", "Bundle Machine CFG Edges",
                /* cfg = */true, /* analysis = */ true)

char &llvm37::EdgeBundlesID = EdgeBundles::ID;

void EdgeBundles::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  MachineFunctionPass::getAnalysisUsage(AU);
}

bool EdgeBundles::runOnMachineFunction(MachineFunction &mf) {
  MF = &mf;
  EC.clear();
  EC.grow(2 * MF->getNumBlockIDs());

  for (const auto &MBB : *MF) {
    unsigned OutE = 2 * MBB.getNumber() + 1;
    // Join the outgoing bundle with the ingoing bundles of all successors.
    for (MachineBasicBlock::const_succ_iterator SI = MBB.succ_begin(),
           SE = MBB.succ_end(); SI != SE; ++SI)
      EC.join(OutE, 2 * (*SI)->getNumber());
  }
  EC.compress();
  if (ViewEdgeBundles)
    view();

  // Compute the reverse mapping.
  Blocks.clear();
  Blocks.resize(getNumBundles());

  for (unsigned i = 0, e = MF->getNumBlockIDs(); i != e; ++i) {
    unsigned b0 = getBundle(i, 0);
    unsigned b1 = getBundle(i, 1);
    Blocks[b0].push_back(i);
    if (b1 != b0)
      Blocks[b1].push_back(i);
  }

  return false;
}

/// Specialize WriteGraph, the standard implementation won't work.
namespace llvm37 {
template<>
raw_ostream &WriteGraph<>(raw_ostream &O, const EdgeBundles &G,
                          bool ShortNames,
                          const Twine &Title) {
  const MachineFunction *MF = G.getMachineFunction();

  O << "digraph {\n";
  for (const auto &MBB : *MF) {
    unsigned BB = MBB.getNumber();
    O << "\t\"BB#" << BB << "\" [ shape=box ]\n"
      << '\t' << G.getBundle(BB, false) << " -> \"BB#" << BB << "\"\n"
      << "\t\"BB#" << BB << "\" -> " << G.getBundle(BB, true) << '\n';
    for (MachineBasicBlock::const_succ_iterator SI = MBB.succ_begin(),
           SE = MBB.succ_end(); SI != SE; ++SI)
      O << "\t\"BB#" << BB << "\" -> \"BB#" << (*SI)->getNumber()
        << "\" [ color=lightgray ]\n";
  }
  O << "}\n";
  return O;
}
}

/// view - Visualize the annotated bipartite CFG with Graphviz.
void EdgeBundles::view() const {
  ViewGraph(*this, "EdgeBundles");
}
