//===- Interval.cpp - Interval class code ---------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file contains the definition of the Interval class, which represents a
// partition of a control flow graph of some kind.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Analysis/Interval.h"
#include "llvm37/IR/BasicBlock.h"
#include "llvm37/IR/CFG.h"
#include "llvm37/Support/raw_ostream.h"
#include <algorithm>

using namespace llvm37;

//===----------------------------------------------------------------------===//
// Interval Implementation
//===----------------------------------------------------------------------===//

// isLoop - Find out if there is a back edge in this interval...
//
bool Interval::isLoop() const {
  // There is a loop in this interval iff one of the predecessors of the header
  // node lives in the interval.
  for (::pred_iterator I = ::pred_begin(HeaderNode), E = ::pred_end(HeaderNode);
       I != E; ++I)
    if (contains(*I))
      return true;
  return false;
}


void Interval::print(raw_ostream &OS) const {
  OS << "-------------------------------------------------------------\n"
       << "Interval Contents:\n";

  // Print out all of the basic blocks in the interval...
  for (std::vector<BasicBlock*>::const_iterator I = Nodes.begin(),
         E = Nodes.end(); I != E; ++I)
    OS << **I << "\n";

  OS << "Interval Predecessors:\n";
  for (std::vector<BasicBlock*>::const_iterator I = Predecessors.begin(),
         E = Predecessors.end(); I != E; ++I)
    OS << **I << "\n";

  OS << "Interval Successors:\n";
  for (std::vector<BasicBlock*>::const_iterator I = Successors.begin(),
         E = Successors.end(); I != E; ++I)
    OS << **I << "\n";
}
