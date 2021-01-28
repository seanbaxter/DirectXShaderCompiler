//===-- MCAsmParserExtension.cpp - Asm Parser Hooks -----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCParser/MCAsmParserExtension.h"
using namespace llvm37;

MCAsmParserExtension::MCAsmParserExtension() :
  BracketExpressionsSupported(false) {
}

MCAsmParserExtension::~MCAsmParserExtension() {
}

void MCAsmParserExtension::Initialize(MCAsmParser &Parser) {
  this->Parser = &Parser;
}
