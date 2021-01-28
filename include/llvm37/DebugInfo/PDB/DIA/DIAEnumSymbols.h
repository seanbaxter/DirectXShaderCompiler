//==- DIAEnumSymbols.h - DIA Symbol Enumerator impl --------------*- C++ -*-==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_DIA_DIAENUMSYMBOLS_H
#define LLVM37_DEBUGINFO_PDB_DIA_DIAENUMSYMBOLS_H

#include "DIASupport.h"
#include "llvm37/DebugInfo/PDB/IPDBEnumChildren.h"

namespace llvm37 {

class DIASession;

class DIAEnumSymbols : public IPDBEnumChildren<PDBSymbol> {
public:
  explicit DIAEnumSymbols(const DIASession &Session,
                          CComPtr<IDiaEnumSymbols> DiaEnumerator);

  uint32_t getChildCount() const override;
  std::unique_ptr<PDBSymbol> getChildAtIndex(uint32_t Index) const override;
  std::unique_ptr<PDBSymbol> getNext() override;
  void reset() override;
  DIAEnumSymbols *clone() const override;

private:
  const DIASession &Session;
  CComPtr<IDiaEnumSymbols> Enumerator;
};
}

#endif
