//===-- llvm37/CodeGen/ExpandISelPseudos.cpp ----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Expand Pseudo-instructions produced by ISel. These are usually to allow
// the expansion to contain control flow, such as a conditional move
// implemented with a conditional branch and a phi, or an atomic operation
// implemented with a loop.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/Passes.h"
#include "llvm37/CodeGen/MachineFunction.h"
#include "llvm37/CodeGen/MachineFunctionPass.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Target/TargetLowering.h"
#include "llvm37/Target/TargetSubtargetInfo.h"
using namespace llvm37;

#define DEBUG_TYPE "expand-isel-pseudos"

namespace {
  class ExpandISelPseudos : public MachineFunctionPass {
  public:
    static char ID; // Pass identification, replacement for typeid
    ExpandISelPseudos() : MachineFunctionPass(ID) {}

  private:
    bool runOnMachineFunction(MachineFunction &MF) override;

    void getAnalysisUsage(AnalysisUsage &AU) const override {
      MachineFunctionPass::getAnalysisUsage(AU);
    }
  };
} // end anonymous namespace

char ExpandISelPseudos::ID = 0;
char &llvm37::ExpandISelPseudosID = ExpandISelPseudos::ID;
INITIALIZELLVM37_PASS(ExpandISelPseudos, "expand-isel-pseudos",
                "Expand ISel Pseudo-instructions", false, false)

bool ExpandISelPseudos::runOnMachineFunction(MachineFunction &MF) {
  bool Changed = false;
  const TargetLowering *TLI = MF.getSubtarget().getTargetLowering();

  // Iterate through each instruction in the function, looking for pseudos.
  for (MachineFunction::iterator I = MF.begin(), E = MF.end(); I != E; ++I) {
    MachineBasicBlock *MBB = I;
    for (MachineBasicBlock::iterator MBBI = MBB->begin(), MBBE = MBB->end();
         MBBI != MBBE; ) {
      MachineInstr *MI = MBBI++;

      // If MI is a pseudo, expand it.
      if (MI->usesCustomInsertionHook()) {
        Changed = true;
        MachineBasicBlock *NewMBB =
          TLI->EmitInstrWithCustomInserter(MI, MBB);
        // The expansion may involve new basic blocks.
        if (NewMBB != MBB) {
          MBB = NewMBB;
          I = NewMBB;
          MBBI = NewMBB->begin();
          MBBE = NewMBB->end();
        }
      }
    }
  }

  return Changed;
}
