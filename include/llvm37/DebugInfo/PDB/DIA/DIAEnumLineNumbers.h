//==- DIAEnumLineNumbers.h - DIA Line Number Enumerator impl -----*- C++ -*-==//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_DIA_DIAENUMLINENUMBERS_H
#define LLVM37_DEBUGINFO_PDB_DIA_DIAENUMLINENUMBERS_H

#include "DIASupport.h"
#include "llvm37/DebugInfo/PDB/IPDBEnumChildren.h"

namespace llvm37 {

class IPDBLineNumber;

class DIAEnumLineNumbers : public IPDBEnumChildren<IPDBLineNumber> {
public:
  explicit DIAEnumLineNumbers(CComPtr<IDiaEnumLineNumbers> DiaEnumerator);

  uint32_t getChildCount() const override;
  ChildTypePtr getChildAtIndex(uint32_t Index) const override;
  ChildTypePtr getNext() override;
  void reset() override;
  DIAEnumLineNumbers *clone() const override;

private:
  CComPtr<IDiaEnumLineNumbers> Enumerator;
};
}

#endif
