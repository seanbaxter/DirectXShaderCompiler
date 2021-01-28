//===-- MachineFunctionPrinterPass.cpp ------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// MachineFunctionPrinterPass implementation.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/Passes.h"
#include "llvm37/CodeGen/MachineFunction.h"
#include "llvm37/CodeGen/MachineFunctionPass.h"
#include "llvm37/CodeGen/SlotIndexes.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/raw_ostream.h"

using namespace llvm37;

namespace {
/// MachineFunctionPrinterPass - This is a pass to dump the IR of a
/// MachineFunction.
///
struct MachineFunctionPrinterPass : public MachineFunctionPass {
  static char ID;

  raw_ostream &OS;
  const std::string Banner;

  MachineFunctionPrinterPass() : MachineFunctionPass(ID), OS(dbgs()) { }
  MachineFunctionPrinterPass(raw_ostream &os, const std::string &banner) 
      : MachineFunctionPass(ID), OS(os), Banner(banner) {}

  const char *getPassName() const override { return "MachineFunction Printer"; }

  void getAnalysisUsage(AnalysisUsage &AU) const override {
    AU.setPreservesAll();
    MachineFunctionPass::getAnalysisUsage(AU);
  }

  bool runOnMachineFunction(MachineFunction &MF) override {
    OS << "# " << Banner << ":\n";
    MF.print(OS, getAnalysisIfAvailable<SlotIndexes>());
    return false;
  }
};

char MachineFunctionPrinterPass::ID = 0;
}

char &llvm37::MachineFunctionPrinterPassID = MachineFunctionPrinterPass::ID;
INITIALIZELLVM37_PASS(MachineFunctionPrinterPass, "machineinstr-printer",
                "Machine Function Printer", false, false)

namespace llvm37 {
/// Returns a newly-created MachineFunction Printer pass. The
/// default banner is empty.
///
MachineFunctionPass *createMachineFunctionPrinterPass(raw_ostream &OS,
                                                      const std::string &Banner){
  return new MachineFunctionPrinterPass(OS, Banner);
}

}
