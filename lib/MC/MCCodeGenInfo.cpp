//===-- MCCodeGenInfo.cpp - Target CodeGen Info -----------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file tracks information about the target which can affect codegen,
// asm parsing, and asm printing. For example, relocation model.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCCodeGenInfo.h"
using namespace llvm37;

void MCCodeGenInfo::initMCCodeGenInfo(Reloc::Model RM, CodeModel::Model CM,
                                      CodeGenOpt::Level OL) {
  RelocationModel = RM;
  CMModel = CM;
  OptLevel = OL;
}
