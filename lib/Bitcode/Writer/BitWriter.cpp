//===-- BitWriter.cpp -----------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/BitWriter.h"
#include "llvm37/Bitcode/ReaderWriter.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/FileSystem.h"
#include "llvm37/Support/raw_ostream.h"
using namespace llvm37;


/*===-- Operations on modules ---------------------------------------------===*/

int LLVM37WriteBitcodeToFile(LLVM37ModuleRef M, const char *Path) {
  std::error_code EC;
  raw_fd_ostream OS(Path, EC, sys::fs::F_None);

  if (EC)
    return -1;

  WriteBitcodeToFile(unwrap(M), OS);
  return 0;
}

int LLVM37WriteBitcodeToFD(LLVM37ModuleRef M, int FD, int ShouldClose,
                         int Unbuffered) {
  raw_fd_ostream OS(FD, ShouldClose, Unbuffered);

  WriteBitcodeToFile(unwrap(M), OS);
  return 0;
}

int LLVM37WriteBitcodeToFileHandle(LLVM37ModuleRef M, int FileHandle) {
  return LLVM37WriteBitcodeToFD(M, FileHandle, true, false);
}

LLVM37MemoryBufferRef LLVM37WriteBitcodeToMemoryBuffer(LLVM37ModuleRef M) {
  std::string Data;
  raw_string_ostream OS(Data);

  WriteBitcodeToFile(unwrap(M), OS);
  return wrap(MemoryBuffer::getMemBufferCopy(OS.str()).release());
}
