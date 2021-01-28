//==- DIAEnumDebugStreams.h - DIA Debug Stream Enumerator impl ---*- C++ -*-==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_DIA_DIAENUMDEBUGSTREAMS_H
#define LLVM37_DEBUGINFO_PDB_DIA_DIAENUMDEBUGSTREAMS_H

#include "DIASupport.h"
#include "llvm37/DebugInfo/PDB/IPDBEnumChildren.h"

namespace llvm37 {

class IPDBDataStream;

class DIAEnumDebugStreams : public IPDBEnumChildren<IPDBDataStream> {
public:
  explicit DIAEnumDebugStreams(CComPtr<IDiaEnumDebugStreams> DiaEnumerator);

  uint32_t getChildCount() const override;
  ChildTypePtr getChildAtIndex(uint32_t Index) const override;
  ChildTypePtr getNext() override;
  void reset() override;
  DIAEnumDebugStreams *clone() const override;

private:
  CComPtr<IDiaEnumDebugStreams> Enumerator;
};
}

#endif
