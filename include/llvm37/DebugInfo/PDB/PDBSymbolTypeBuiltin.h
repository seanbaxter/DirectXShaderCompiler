//===- PDBSymbolTypeBuiltin.h - builtin type information --------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_PDBSYMBOLTYPEBUILTIN_H
#define LLVM37_DEBUGINFO_PDB_PDBSYMBOLTYPEBUILTIN_H

#include "PDBSymbol.h"
#include "PDBTypes.h"

namespace llvm37 {

class raw_ostream;

class PDBSymbolTypeBuiltin : public PDBSymbol {
public:
  PDBSymbolTypeBuiltin(const IPDBSession &PDBSession,
                       std::unique_ptr<IPDBRawSymbol> Symbol);

  DECLARE_PDB_SYMBOL_CONCRETE_TYPE(PDB_SymType::BuiltinType)

  void dump(PDBSymDumper &Dumper) const override;

  FORWARD_SYMBOL_METHOD(getBuiltinType)
  FORWARD_SYMBOL_METHOD(isConstType)
  FORWARD_SYMBOL_METHOD(getLength)
  FORWARD_SYMBOL_METHOD(getLexicalParentId)
  FORWARD_SYMBOL_METHOD(getSymIndexId)
  FORWARD_SYMBOL_METHOD(isUnalignedType)
  FORWARD_SYMBOL_METHOD(isVolatileType)
};

} // namespace llvm37

#endif // LLVM37_DEBUGINFO_PDB_PDBSYMBOLTYPEBUILTIN_H
