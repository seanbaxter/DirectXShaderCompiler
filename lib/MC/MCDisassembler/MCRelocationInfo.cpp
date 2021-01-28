//==-- MCRelocationInfo.cpp ------------------------------------------------==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCRelocationInfo.h"
#include "llvm37-c/Disassembler.h"
#include "llvm37/Object/ObjectFile.h"
#include "llvm37/Support/TargetRegistry.h"

using namespace llvm37;

MCRelocationInfo::MCRelocationInfo(MCContext &Ctx)
  : Ctx(Ctx) {
}

MCRelocationInfo::~MCRelocationInfo() {
}

const MCExpr *
MCRelocationInfo::createExprForRelocation(object::RelocationRef Rel) {
  return nullptr;
}

const MCExpr *
MCRelocationInfo::createExprForCAPIVariantKind(const MCExpr *SubExpr,
                                               unsigned VariantKind) {
  if (VariantKind != LLVM37Disassembler_VariantKind_None)
    return nullptr;
  return SubExpr;
}

MCRelocationInfo *llvm37::createMCRelocationInfo(const Triple &TT,
                                               MCContext &Ctx) {
  return new MCRelocationInfo(Ctx);
}
