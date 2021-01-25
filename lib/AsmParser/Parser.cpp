//===- Parser.cpp - Main dispatch module for the Parser library -----------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This library implements the functionality defined in llvm/AsmParser/Parser.h
//
//===----------------------------------------------------------------------===//

#include "llvm37/AsmParser/Parser.h"
#include "LLParser.h"
#include "llvm37/ADT/STLExtras.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/SourceMgr.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstring>
#include <system_error>
using namespace llvm37;

bool llvm37::parseAssemblyInto(MemoryBufferRef F, Module &M, SMDiagnostic &Err,
                             SlotMapping *Slots) {
  SourceMgr SM;
  std::unique_ptr<MemoryBuffer> Buf = MemoryBuffer::getMemBuffer(F);
  SM.AddNewSourceBuffer(std::move(Buf), SMLoc());

  return LLParser(F.getBuffer(), SM, Err, &M, Slots).Run();
}

std::unique_ptr<Module> llvm37::parseAssembly(MemoryBufferRef F,
                                            SMDiagnostic &Err,
                                            LLVMContext &Context,
                                            SlotMapping *Slots) {
  std::unique_ptr<Module> M =
      make_unique<Module>(F.getBufferIdentifier(), Context);

  if (parseAssemblyInto(F, *M, Err, Slots))
    return nullptr;

  return M;
}

std::unique_ptr<Module> llvm37::parseAssemblyFile(StringRef Filename,
                                                SMDiagnostic &Err,
                                                LLVMContext &Context,
                                                SlotMapping *Slots) {
  ErrorOr<std::unique_ptr<MemoryBuffer>> FileOrErr =
      MemoryBuffer::getFileOrSTDIN(Filename);
  if (std::error_code EC = FileOrErr.getError()) {
    Err = SMDiagnostic(Filename, SourceMgr::DK_Error,
                       "Could not open input file: " + EC.message());
    return nullptr;
  }

  return parseAssembly(FileOrErr.get()->getMemBufferRef(), Err, Context, Slots);
}

std::unique_ptr<Module> llvm37::parseAssemblyString(StringRef AsmString,
                                                  SMDiagnostic &Err,
                                                  LLVMContext &Context,
                                                  SlotMapping *Slots) {
  MemoryBufferRef F(AsmString, "<string>");
  return parseAssembly(F, Err, Context, Slots);
}
