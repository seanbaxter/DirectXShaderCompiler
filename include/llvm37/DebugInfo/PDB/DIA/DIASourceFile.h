//===- DIASourceFile.h - DIA implementation of IPDBSourceFile ---*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_DIA_DIASOURCEFILE_H
#define LLVM37_DEBUGINFO_PDB_DIA_DIASOURCEFILE_H

#include "DIASupport.h"
#include "llvm37/DebugInfo/PDB/IPDBSourceFile.h"

namespace llvm37 {
class DIASession;

class DIASourceFile : public IPDBSourceFile {
public:
  explicit DIASourceFile(const DIASession &Session,
                         CComPtr<IDiaSourceFile> DiaSourceFile);

  std::string getFileName() const override;
  uint32_t getUniqueId() const override;
  std::string getChecksum() const override;
  PDB_Checksum getChecksumType() const override;
  std::unique_ptr<IPDBEnumSymbols> getCompilands() const override;

private:
  const DIASession &Session;
  CComPtr<IDiaSourceFile> SourceFile;
};
}

#endif
