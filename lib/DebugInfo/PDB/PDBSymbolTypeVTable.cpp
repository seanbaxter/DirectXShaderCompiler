//===- PDBSymbolTypeVTable.cpp - --------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/DebugInfo/PDB/PDBSymbolTypeVTable.h"

#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

#include <utility>

using namespace llvm37;

PDBSymbolTypeVTable::PDBSymbolTypeVTable(const IPDBSession &PDBSession,
                                         std::unique_ptr<IPDBRawSymbol> Symbol)
    : PDBSymbol(PDBSession, std::move(Symbol)) {}

void PDBSymbolTypeVTable::dump(PDBSymDumper &Dumper) const {
  Dumper.dump(*this);
}
