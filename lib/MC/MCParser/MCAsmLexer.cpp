//===-- MCAsmLexer.cpp - Abstract Asm Lexer Interface ---------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCParser/MCAsmLexer.h"
#include "llvm37/Support/SourceMgr.h"

using namespace llvm37;

MCAsmLexer::MCAsmLexer() : CurTok(AsmToken::Error, StringRef()),
                           TokStart(nullptr), SkipSpace(true) {
}

MCAsmLexer::~MCAsmLexer() {
}

SMLoc MCAsmLexer::getLoc() const {
  return SMLoc::getFromPointer(TokStart);
}

SMLoc AsmToken::getLoc() const {
  return SMLoc::getFromPointer(Str.data());
}

SMLoc AsmToken::getEndLoc() const {
  return SMLoc::getFromPointer(Str.data() + Str.size());
}

SMRange AsmToken::getLocRange() const {
  return SMRange(getLoc(), getEndLoc());
}
