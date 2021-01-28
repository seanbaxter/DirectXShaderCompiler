//===-- MachineFunctionAnalysis.h - Owner of MachineFunctions ----*-C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the MachineFunctionAnalysis class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CODEGEN_MACHINEFUNCTIONANALYSIS_H
#define LLVM37_CODEGEN_MACHINEFUNCTIONANALYSIS_H

#include "llvm37/Pass.h"

namespace llvm37 {

class MachineFunction;
class MachineFunctionInitializer;
class TargetMachine;

/// MachineFunctionAnalysis - This class is a Pass that manages a
/// MachineFunction object.
struct MachineFunctionAnalysis : public FunctionPass {
private:
  const TargetMachine &TM;
  MachineFunction *MF;
  unsigned NextFnNum;
  MachineFunctionInitializer *MFInitializer;

public:
  static char ID;
  explicit MachineFunctionAnalysis(const TargetMachine &tm,
                                   MachineFunctionInitializer *MFInitializer);
  ~MachineFunctionAnalysis() override;

  MachineFunction &getMF() const { return *MF; }

  const char* getPassName() const override {
    return "Machine Function Analysis";
  }

private:
  bool doInitialization(Module &M) override;
  bool runOnFunction(Function &F) override;
  void releaseMemory() override;
  void getAnalysisUsage(AnalysisUsage &AU) const override;
};

} // End llvm37 namespace

#endif
