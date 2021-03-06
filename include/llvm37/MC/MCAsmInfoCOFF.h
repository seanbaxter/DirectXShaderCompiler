//===-- MCAsmInfoCOFF.h - COFF asm properties -------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCASMINFOCOFF_H
#define LLVM37_MC_MCASMINFOCOFF_H

#include "llvm37/MC/MCAsmInfo.h"

namespace llvm37 {
  class MCAsmInfoCOFF : public MCAsmInfo {
    virtual void anchor();
  protected:
    explicit MCAsmInfoCOFF();
  };

  class MCAsmInfoMicrosoft : public MCAsmInfoCOFF {
    void anchor() override;
  protected:
    explicit MCAsmInfoMicrosoft();
  };

  class MCAsmInfoGNUCOFF : public MCAsmInfoCOFF {
    void anchor() override;
  protected:
    explicit MCAsmInfoGNUCOFF();
  };
}


#endif // LLVM37_MC_MCASMINFOCOFF_H
