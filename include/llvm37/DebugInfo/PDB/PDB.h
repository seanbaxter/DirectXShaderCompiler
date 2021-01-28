//===- PDB.h - base header file for creating a PDB reader -------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_DEBUGINFO_PDB_PDB_H
#define LLVM37_DEBUGINFO_PDB_PDB_H

#include "PDBTypes.h"
#include <memory>

namespace llvm37 {
class StringRef;

PDB_ErrorCode loadDataForPDB(PDB_ReaderType Type, StringRef Path,
                             std::unique_ptr<IPDBSession> &Session);

PDB_ErrorCode loadDataForEXE(PDB_ReaderType Type, StringRef Path,
                             std::unique_ptr<IPDBSession> &Session);
}

#endif
