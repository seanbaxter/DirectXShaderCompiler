//===-- MCDisassembler.cpp - Disassembler interface -----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCDisassembler.h"
#include "llvm37/MC/MCExternalSymbolizer.h"
#include "llvm37/Support/raw_ostream.h"

using namespace llvm37;

MCDisassembler::~MCDisassembler() {
}

bool MCDisassembler::tryAddingSymbolicOperand(MCInst &Inst, int64_t Value,
                                              uint64_t Address, bool IsBranch,
                                              uint64_t Offset,
                                              uint64_t InstSize) const {
  raw_ostream &cStream = CommentStream ? *CommentStream : nulls();
  if (Symbolizer)
    return Symbolizer->tryAddingSymbolicOperand(Inst, cStream, Value, Address,
                                                IsBranch, Offset, InstSize);
  return false;
}

void MCDisassembler::tryAddingPcLoadReferenceComment(int64_t Value,
                                                     uint64_t Address) const {
  raw_ostream &cStream = CommentStream ? *CommentStream : nulls();
  if (Symbolizer)
    Symbolizer->tryAddingPcLoadReferenceComment(cStream, Value, Address);
}

void MCDisassembler::setSymbolizer(std::unique_ptr<MCSymbolizer> Symzer) {
  Symbolizer = std::move(Symzer);
}
