//===- PDBSymbolTypeEnum.cpp - --------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/DebugInfo/PDB/PDBSymbolTypeEnum.h"

#include "llvm37/DebugInfo/PDB/IPDBSession.h"
#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"
#include "llvm37/DebugInfo/PDB/PDBSymbolTypeBuiltin.h"
#include "llvm37/DebugInfo/PDB/PDBSymbolTypeUDT.h"

#include <utility>

using namespace llvm37;

PDBSymbolTypeEnum::PDBSymbolTypeEnum(const IPDBSession &PDBSession,
                                     std::unique_ptr<IPDBRawSymbol> Symbol)
    : PDBSymbol(PDBSession, std::move(Symbol)) {}

std::unique_ptr<PDBSymbolTypeUDT> PDBSymbolTypeEnum::getClassParent() const {
  return Session.getConcreteSymbolById<PDBSymbolTypeUDT>(getClassParentId());
}

std::unique_ptr<PDBSymbolTypeBuiltin>
PDBSymbolTypeEnum::getUnderlyingType() const {
  return Session.getConcreteSymbolById<PDBSymbolTypeBuiltin>(getTypeId());
}

void PDBSymbolTypeEnum::dump(PDBSymDumper &Dumper) const { Dumper.dump(*this); }
