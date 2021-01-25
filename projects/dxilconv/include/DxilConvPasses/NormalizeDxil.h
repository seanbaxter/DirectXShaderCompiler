///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// NormalizeDxil.cpp                                                         //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Normalize DXIL transformation.                                            //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "llvm37/Pass.h"


namespace llvm37 {
  class Function;
  class PassRegistry;
  class FunctionPass;
  class Function;
  class DominatorTree;


  llvm37::FunctionPass *createNormalizeDxilPass();
  void initializeNormalizeDxilPassPass(llvm37::PassRegistry&);

  class NormalizeDxil {
  public:
    NormalizeDxil(Function &F, DominatorTree &DT) : m_function(F), m_dominatorTree(DT) {}

    virtual bool Run();

  protected:
    Function &m_function;
    DominatorTree &m_dominatorTree;
  };

  // The legacy pass manager's analysis pass to normalize dxil ir.
  class NormalizeDxilPass : public FunctionPass {
  public:
    static char ID; // Pass identification, replacement for typeid

    NormalizeDxilPass() : FunctionPass(ID) {
      initializeNormalizeDxilPassPass(*PassRegistry::getPassRegistry());
    }

    // Normalize incoming dxil ir.
    bool runOnFunction(Function &F) override;

    virtual const char *getPassName() const override { return "Normalize Dxil"; }

    void getAnalysisUsage(AnalysisUsage &AU) const override;
  };
}
