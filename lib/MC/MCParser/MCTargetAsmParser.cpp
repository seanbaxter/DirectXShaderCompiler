//===-- MCTargetAsmParser.cpp - Target Assembly Parser ---------------------==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCTargetAsmParser.h"
using namespace llvm37;

MCTargetAsmParser::MCTargetAsmParser()
  : AvailableFeatures(0), ParsingInlineAsm(false)
{
}

MCTargetAsmParser::~MCTargetAsmParser() {
}
