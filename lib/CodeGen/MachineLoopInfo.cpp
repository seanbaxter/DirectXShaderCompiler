//===- MachineLoopInfo.cpp - Natural Loop Calculator ----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the MachineLoopInfo class that is used to identify natural
// loops and determine the loop depth of various nodes of the CFG.  Note that
// the loops identified may actually be several natural loops that share the
// same header node... not just a single natural loop.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/MachineLoopInfo.h"
#include "llvm37/Analysis/LoopInfoImpl.h"
#include "llvm37/CodeGen/MachineDominators.h"
#include "llvm37/CodeGen/Passes.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/raw_ostream.h"
using namespace llvm37;

// Explicitly instantiate methods in LoopInfoImpl.h for MI-level Loops.
template class llvm37::LoopBase<MachineBasicBlock, MachineLoop>;
template class llvm37::LoopInfoBase<MachineBasicBlock, MachineLoop>;

char MachineLoopInfo::ID = 0;
INITIALIZELLVM37_PASS_BEGIN(MachineLoopInfo, "machine-loops",
                "Machine Natural Loop Construction", true, true)
INITIALIZELLVM37_PASS_DEPENDENCY(MachineDominatorTree)
INITIALIZELLVM37_PASS_END(MachineLoopInfo, "machine-loops",
                "Machine Natural Loop Construction", true, true)

char &llvm37::MachineLoopInfoID = MachineLoopInfo::ID;

bool MachineLoopInfo::runOnMachineFunction(MachineFunction &) {
  releaseMemory();
  LI.Analyze(getAnalysis<MachineDominatorTree>().getBase());
  return false;
}

void MachineLoopInfo::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addRequired<MachineDominatorTree>();
  MachineFunctionPass::getAnalysisUsage(AU);
}

MachineBasicBlock *MachineLoop::getTopBlock() {
  MachineBasicBlock *TopMBB = getHeader();
  MachineFunction::iterator Begin = TopMBB->getParent()->begin();
  if (TopMBB != Begin) {
    MachineBasicBlock *PriorMBB = std::prev(MachineFunction::iterator(TopMBB));
    while (contains(PriorMBB)) {
      TopMBB = PriorMBB;
      if (TopMBB == Begin) break;
      PriorMBB = std::prev(MachineFunction::iterator(TopMBB));
    }
  }
  return TopMBB;
}

MachineBasicBlock *MachineLoop::getBottomBlock() {
  MachineBasicBlock *BotMBB = getHeader();
  MachineFunction::iterator End = BotMBB->getParent()->end();
  if (BotMBB != std::prev(End)) {
    MachineBasicBlock *NextMBB = std::next(MachineFunction::iterator(BotMBB));
    while (contains(NextMBB)) {
      BotMBB = NextMBB;
      if (BotMBB == std::next(MachineFunction::iterator(BotMBB))) break;
      NextMBB = std::next(MachineFunction::iterator(BotMBB));
    }
  }
  return BotMBB;
}

#if !defined(NDEBUG) || defined(LLVM37_ENABLE_DUMP)
void MachineLoop::dump() const {
  print(dbgs());
}
#endif
