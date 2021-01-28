//===---- MCAsmInfoDarwin.h - Darwin asm properties -------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines target asm properties related what form asm statements
// should take in general on Darwin-based targets
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCASMINFODARWIN_H
#define LLVM37_MC_MCASMINFODARWIN_H

#include "llvm37/MC/MCAsmInfo.h"

namespace llvm37 {
  class MCAsmInfoDarwin : public MCAsmInfo {
  public:
    explicit MCAsmInfoDarwin();
    bool isSectionAtomizableBySymbols(const MCSection &Section) const override;
  };
}


#endif // LLVM37_MC_MCASMINFODARWIN_H
