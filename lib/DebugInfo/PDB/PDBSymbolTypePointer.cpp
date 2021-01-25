//===- PDBSymbolTypePointer.cpp -----------------------------------*- C++ -===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/DebugInfo/PDB/PDBSymbolTypePointer.h"

#include "llvm37/DebugInfo/PDB/IPDBSession.h"
#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

#include <utility>

using namespace llvm37;

PDBSymbolTypePointer::PDBSymbolTypePointer(
    const IPDBSession &PDBSession, std::unique_ptr<IPDBRawSymbol> Symbol)
    : PDBSymbol(PDBSession, std::move(Symbol)) {}

std::unique_ptr<PDBSymbol> PDBSymbolTypePointer::getPointeeType() const {
  return Session.getSymbolById(getTypeId());
}

void PDBSymbolTypePointer::dump(PDBSymDumper &Dumper) const {
  Dumper.dump(*this);
}
