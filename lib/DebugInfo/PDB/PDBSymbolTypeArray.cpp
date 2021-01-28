//===- PDBSymbolTypeArray.cpp - ---------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/DebugInfo/PDB/PDBSymbolTypeArray.h"

#include "llvm37/DebugInfo/PDB/IPDBSession.h"
#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

#include <utility>

using namespace llvm37;

PDBSymbolTypeArray::PDBSymbolTypeArray(const IPDBSession &PDBSession,
                                       std::unique_ptr<IPDBRawSymbol> Symbol)
    : PDBSymbol(PDBSession, std::move(Symbol)) {}

std::unique_ptr<PDBSymbol> PDBSymbolTypeArray::getElementType() const {
  return Session.getSymbolById(getTypeId());
}

void PDBSymbolTypeArray::dump(PDBSymDumper &Dumper) const {
  Dumper.dump(*this);
}
