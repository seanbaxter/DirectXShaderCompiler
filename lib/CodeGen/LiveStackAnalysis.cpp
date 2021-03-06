//===-- LiveStackAnalysis.cpp - Live Stack Slot Analysis ------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the live stack slot analysis pass. It is analogous to
// live interval analysis except it's analyzing liveness of stack slots rather
// than registers.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/LiveStackAnalysis.h"
#include "llvm37/ADT/Statistic.h"
#include "llvm37/CodeGen/LiveIntervalAnalysis.h"
#include "llvm37/CodeGen/Passes.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/raw_ostream.h"
#include "llvm37/Target/TargetRegisterInfo.h"
#include "llvm37/Target/TargetSubtargetInfo.h"
#include <limits>
using namespace llvm37;

#define DEBUG_TYPE "livestacks"

char LiveStacks::ID = 0;
INITIALIZELLVM37_PASS_BEGIN(LiveStacks, "livestacks",
                "Live Stack Slot Analysis", false, false)
INITIALIZELLVM37_PASS_DEPENDENCY(SlotIndexes)
INITIALIZELLVM37_PASS_END(LiveStacks, "livestacks",
                "Live Stack Slot Analysis", false, false)

char &llvm37::LiveStacksID = LiveStacks::ID;

void LiveStacks::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesAll();
  AU.addPreserved<SlotIndexes>();
  AU.addRequiredTransitive<SlotIndexes>();
  MachineFunctionPass::getAnalysisUsage(AU);
}

void LiveStacks::releaseMemory() {
  // Release VNInfo memory regions, VNInfo objects don't need to be dtor'd.
  VNInfoAllocator.Reset();
  S2IMap.clear();
  S2RCMap.clear();
}

bool LiveStacks::runOnMachineFunction(MachineFunction &MF) {
  TRI = MF.getSubtarget().getRegisterInfo();
  // FIXME: No analysis is being done right now. We are relying on the
  // register allocators to provide the information.
  return false;
}

LiveInterval &
LiveStacks::getOrCreateInterval(int Slot, const TargetRegisterClass *RC) {
  assert(Slot >= 0 && "Spill slot indice must be >= 0");
  SS2IntervalMap::iterator I = S2IMap.find(Slot);
  if (I == S2IMap.end()) {
    I = S2IMap.emplace(std::piecewise_construct, std::forward_as_tuple(Slot),
                       std::forward_as_tuple(
                           TargetRegisterInfo::index2StackSlot(Slot), 0.0F))
            .first;
    S2RCMap.insert(std::make_pair(Slot, RC));
  } else {
    // Use the largest common subclass register class.
    const TargetRegisterClass *OldRC = S2RCMap[Slot];
    S2RCMap[Slot] = TRI->getCommonSubClass(OldRC, RC);
  }
  return I->second;
}

/// print - Implement the dump method.
void LiveStacks::print(raw_ostream &OS, const Module*) const {

  OS << "********** INTERVALS **********\n";
  for (const_iterator I = begin(), E = end(); I != E; ++I) {
    I->second.print(OS);
    int Slot = I->first;
    const TargetRegisterClass *RC = getIntervalRegClass(Slot);
    if (RC)
      OS << " [" << TRI->getRegClassName(RC) << "]\n";
    else
      OS << " [Unknown]\n";
  }
}
