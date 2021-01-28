//===- PDBSymbolCustom.h - compiler-specific types --------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_PDBSYMBOLCUSTOM_H
#define LLVM37_DEBUGINFO_PDB_PDBSYMBOLCUSTOM_H

#include "PDBSymbol.h"
#include "PDBTypes.h"
#include "llvm37/ADT/SmallVector.h"

namespace llvm37 {

class raw_ostream;

/// PDBSymbolCustom represents symbols that are compiler-specific and do not
/// fit anywhere else in the lexical hierarchy.
/// https://msdn.microsoft.com/en-us/library/d88sf09h.aspx
class PDBSymbolCustom : public PDBSymbol {
public:
  PDBSymbolCustom(const IPDBSession &PDBSession,
                  std::unique_ptr<IPDBRawSymbol> CustomSymbol);

  DECLARE_PDB_SYMBOL_CONCRETE_TYPE(PDB_SymType::Custom)

  void dump(PDBSymDumper &Dumper) const override;

  void getDataBytes(llvm37::SmallVector<uint8_t, 32> &bytes);
  FORWARD_SYMBOL_METHOD(getSymIndexId)
};

} // namespace llvm37

#endif // LLVM37_DEBUGINFO_PDB_PDBSYMBOLCUSTOM_H
