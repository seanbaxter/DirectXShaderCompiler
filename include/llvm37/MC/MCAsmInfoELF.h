//===-- llvm37/MC/MCAsmInfoELF.h - ELF Asm info -------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_MC_MCASMINFOELF_H
#define LLVM37_MC_MCASMINFOELF_H

#include "llvm37/MC/MCAsmInfo.h"

namespace llvm37 {
class MCAsmInfoELF : public MCAsmInfo {
  virtual void anchor();
  MCSection *getNonexecutableStackSection(MCContext &Ctx) const final;

protected:
  MCAsmInfoELF();
};
}

#endif
