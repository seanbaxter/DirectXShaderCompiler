//===- DIADataStream.h - DIA implementation of IPDBDataStream ---*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_DIA_DIADATASTREAM_H
#define LLVM37_DEBUGINFO_PDB_DIA_DIADATASTREAM_H

#include "DIASupport.h"
#include "llvm37/DebugInfo/PDB/IPDBDataStream.h"

namespace llvm37 {
class DIADataStream : public IPDBDataStream {
public:
  explicit DIADataStream(CComPtr<IDiaEnumDebugStreamData> DiaStreamData);

  uint32_t getRecordCount() const override;
  std::string getName() const override;
  llvm37::Optional<RecordType> getItemAtIndex(uint32_t Index) const override;
  bool getNext(RecordType &Record) override;
  void reset() override;
  DIADataStream *clone() const override;

private:
  CComPtr<IDiaEnumDebugStreamData> StreamData;
};
}

#endif
