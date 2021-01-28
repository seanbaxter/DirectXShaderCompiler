//===- PDBSymbolCustom.cpp - compiler-specific types ------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/DebugInfo/PDB/PDBSymbolCustom.h"

#include "llvm37/DebugInfo/PDB/IPDBRawSymbol.h"
#include "llvm37/DebugInfo/PDB/PDBSymbol.h"
#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

#include <utility>

using namespace llvm37;

PDBSymbolCustom::PDBSymbolCustom(const IPDBSession &PDBSession,
                                 std::unique_ptr<IPDBRawSymbol> CustomSymbol)
    : PDBSymbol(PDBSession, std::move(CustomSymbol)) {}

void PDBSymbolCustom::getDataBytes(llvm37::SmallVector<uint8_t, 32> &bytes) {
  RawSymbol->getDataBytes(bytes);
}

void PDBSymbolCustom::dump(PDBSymDumper &Dumper) const { Dumper.dump(*this); }
