//===-- llvm37-dwarfdump-fuzzer.cpp - Fuzz the llvm37-dwarfdump tool ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief This file implements a function that runs llvm37-dwarfdump
///  on a single input. This function is then linked into the Fuzzer library.
///
//===----------------------------------------------------------------------===//

#include "llvm37/DebugInfo/DIContext.h"
#include "llvm37/DebugInfo/DWARF/DWARFContext.h"
#include "llvm37/Object/ObjectFile.h"
#include "llvm37/Support/MemoryBuffer.h"
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
using namespace llvm37;
using namespace object;

extern "C" void LLVM37FuzzerTestOneInput(uint8_t *data, size_t size) {
  std::unique_ptr<MemoryBuffer> Buff = MemoryBuffer::getMemBuffer(
      StringRef((const char *)data, size), "", false);

  ErrorOr<std::unique_ptr<ObjectFile>> ObjOrErr =
      ObjectFile::createObjectFile(Buff->getMemBufferRef());
  if (!ObjOrErr)
    return;
  ObjectFile &Obj = *ObjOrErr.get();
  std::unique_ptr<DIContext> DICtx(new DWARFContextInMemory(Obj));
  DICtx->dump(nulls(), DIDT_All);
}
