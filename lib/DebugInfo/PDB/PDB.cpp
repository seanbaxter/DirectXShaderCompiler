//===- PDB.cpp - base header file for creating a PDB reader -----*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/DebugInfo/PDB/PDB.h"

#include "llvm37/ADT/StringRef.h"
#include "llvm37/Config/config.h"
#include "llvm37/DebugInfo/PDB/IPDBSession.h"
#include "llvm37/DebugInfo/PDB/PDB.h"

#if HAVE_DIA_SDK
#include "llvm37/DebugInfo/PDB/DIA/DIASession.h"
#endif

using namespace llvm37;

PDB_ErrorCode llvm37::loadDataForPDB(PDB_ReaderType Type, StringRef Path,
                                   std::unique_ptr<IPDBSession> &Session) {
  // Create the correct concrete instance type based on the value of Type.
#if HAVE_DIA_SDK
  return DIASession::createFromPdb(Path, Session);
#endif
  return PDB_ErrorCode::NoPdbImpl;
}

PDB_ErrorCode llvm37::loadDataForEXE(PDB_ReaderType Type, StringRef Path,
                                   std::unique_ptr<IPDBSession> &Session) {
// Create the correct concrete instance type based on the value of Type.
#if HAVE_DIA_SDK
  return DIASession::createFromExe(Path, Session);
#endif
  return PDB_ErrorCode::NoPdbImpl;
}
