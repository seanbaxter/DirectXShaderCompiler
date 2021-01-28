//===- PDBSymbolUnknown.h - unknown symbol type -----------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_PDBSYMBOLUNKNOWN_H
#define LLVM37_DEBUGINFO_PDB_PDBSYMBOLUNKNOWN_H

#include "PDBSymbol.h"

namespace llvm37 {

class raw_ostream;

class PDBSymbolUnknown : public PDBSymbol {
public:
  PDBSymbolUnknown(const IPDBSession &PDBSession,
                   std::unique_ptr<IPDBRawSymbol> UnknownSymbol);

  void dump(PDBSymDumper &Dumper) const override;

  static bool classof(const PDBSymbol *S) {
    return (S->getSymTag() == PDB_SymType::None ||
            S->getSymTag() >= PDB_SymType::Max);
  }
};

} // namespace llvm37

#endif // LLVM37_DEBUGINFO_PDB_PDBSYMBOLUNKNOWN_H
