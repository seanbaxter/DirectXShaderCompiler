//===-- MCAsmParser.cpp - Abstract Asm Parser Interface -------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCParser/MCAsmParser.h"
#include "llvm37/ADT/Twine.h"
#include "llvm37/MC/MCParser/MCAsmLexer.h"
#include "llvm37/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm37/MC/MCTargetAsmParser.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/SourceMgr.h"
#include "llvm37/Support/raw_ostream.h"
using namespace llvm37;

MCAsmParser::MCAsmParser() : TargetParser(nullptr), ShowParsedOperands(0) {
}

MCAsmParser::~MCAsmParser() {
}

void MCAsmParser::setTargetParser(MCTargetAsmParser &P) {
  assert(!TargetParser && "Target parser is already initialized!");
  TargetParser = &P;
  TargetParser->Initialize(*this);
}

const AsmToken &MCAsmParser::getTok() const {
  return getLexer().getTok();
}

bool MCAsmParser::TokError(const Twine &Msg, ArrayRef<SMRange> Ranges) {
  Error(getLexer().getLoc(), Msg, Ranges);
  return true;
}

bool MCAsmParser::parseExpression(const MCExpr *&Res) {
  SMLoc L;
  return parseExpression(Res, L);
}

void MCParsedAsmOperand::dump() const {
#if !defined(NDEBUG) || defined(LLVM37_ENABLE_DUMP)
  dbgs() << "  " << *this;
#endif
}
