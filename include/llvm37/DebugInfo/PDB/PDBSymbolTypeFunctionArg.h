//===- PDBSymbolTypeFunctionArg.h - function arg type info ------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_PDBSYMBOLTYPEFUNCTIONARG_H
#define LLVM37_DEBUGINFO_PDB_PDBSYMBOLTYPEFUNCTIONARG_H

#include "PDBSymbol.h"
#include "PDBTypes.h"

namespace llvm37 {

class raw_ostream;

class PDBSymbolTypeFunctionArg : public PDBSymbol {
public:
  PDBSymbolTypeFunctionArg(const IPDBSession &PDBSession,
                           std::unique_ptr<IPDBRawSymbol> Symbol);

  DECLARE_PDB_SYMBOL_CONCRETE_TYPE(PDB_SymType::FunctionArg)

  void dump(PDBSymDumper &Dumper) const override;

  FORWARD_SYMBOL_METHOD(getClassParentId)
  FORWARD_SYMBOL_METHOD(getLexicalParentId)
  FORWARD_SYMBOL_METHOD(getSymIndexId)
  FORWARD_SYMBOL_METHOD(getTypeId)
};

} // namespace llvm37

#endif // LLVM37_DEBUGINFO_PDB_PDBSYMBOLTYPEFUNCTIONARG_H
