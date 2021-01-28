//===- PDBSymbolCompilandEnv.h - compiland environment variables *- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_PDBSYMBOLCOMPILANDENV_H
#define LLVM37_DEBUGINFO_PDB_PDBSYMBOLCOMPILANDENV_H

#include "PDBSymbol.h"
#include "PDBTypes.h"

namespace llvm37 {

class raw_ostream;

class PDBSymbolCompilandEnv : public PDBSymbol {
public:
  PDBSymbolCompilandEnv(const IPDBSession &PDBSession,
                        std::unique_ptr<IPDBRawSymbol> Symbol);

  DECLARE_PDB_SYMBOL_CONCRETE_TYPE(PDB_SymType::CompilandEnv)

  void dump(PDBSymDumper &Dumper) const override;

  FORWARD_SYMBOL_METHOD(getLexicalParentId)
  FORWARD_SYMBOL_METHOD(getName)
  FORWARD_SYMBOL_METHOD(getSymIndexId)
  std::string getValue() const;
};

} // namespace llvm37

#endif // LLVM37_DEBUGINFO_PDB_PDBSYMBOLCOMPILANDENV_H
