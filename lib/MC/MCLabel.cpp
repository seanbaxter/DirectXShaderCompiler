//===- lib/MC/MCLabel.cpp - MCLabel implementation ----------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/MC/MCLabel.h"
#include "llvm37/Support/Debug.h"
#include "llvm37/Support/raw_ostream.h"
using namespace llvm37;

void MCLabel::print(raw_ostream &OS) const {
  OS << '"' << getInstance() << '"';
}

#if !defined(NDEBUG) || defined(LLVM37_ENABLE_DUMP)
void MCLabel::dump() const {
  print(dbgs());
}
#endif
