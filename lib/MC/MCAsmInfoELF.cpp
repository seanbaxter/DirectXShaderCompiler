//===-- MCAsmInfoELF.cpp - ELF asm properties -------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines target asm properties related what form asm statements
// should take in general on ELF-based targets
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCAsmInfoELF.h"
#include "llvm37/MC/MCContext.h"
#include "llvm37/MC/MCSectionELF.h"
#include "llvm37/Support/ELF.h"
using namespace llvm37;

void MCAsmInfoELF::anchor() { }

MCSection *MCAsmInfoELF::getNonexecutableStackSection(MCContext &Ctx) const {
  return Ctx.getELFSection(".note.GNU-stack", ELF::SHT_PROGBITS, 0);
}

MCAsmInfoELF::MCAsmInfoELF() {
  HasIdentDirective = true;
  WeakRefDirective = "\t.weak\t";
  PrivateGlobalPrefix = ".L";
  PrivateLabelPrefix = ".L";
}
