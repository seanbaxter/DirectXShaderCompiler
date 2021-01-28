//===-- BitReader.cpp -----------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/BitReader.h"
#include "llvm37/Bitcode/ReaderWriter.h"
#include "llvm37/IR/DiagnosticPrinter.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Support/MemoryBuffer.h"
#include "llvm37/Support/raw_ostream.h"
#include <cstring>
#include <string>

using namespace llvm37;

/* Builds a module from the bitcode in the specified memory buffer, returning a
   reference to the module via the OutModule parameter. Returns 0 on success.
   Optionally returns a human-readable error message via OutMessage. */
LLVM37Bool LLVM37ParseBitcode(LLVM37MemoryBufferRef MemBuf,
                          LLVM37ModuleRef *OutModule, char **OutMessage) {
  return LLVM37ParseBitcodeInContext(wrap(&getGlobalContext()), MemBuf, OutModule,
                                   OutMessage);
}

LLVM37Bool LLVM37ParseBitcodeInContext(LLVM37ContextRef ContextRef,
                                   LLVM37MemoryBufferRef MemBuf,
                                   LLVM37ModuleRef *OutModule,
                                   char **OutMessage) {
  MemoryBufferRef Buf = unwrap(MemBuf)->getMemBufferRef();
  LLVM37Context &Ctx = *unwrap(ContextRef);

  std::string Message;
  raw_string_ostream Stream(Message);
  DiagnosticPrinterRawOStream DP(Stream);

  ErrorOr<std::unique_ptr<Module>> ModuleOrErr = parseBitcodeFile(
      Buf, Ctx, [&](const DiagnosticInfo &DI) { DI.print(DP); });
  if (ModuleOrErr.getError()) {
    if (OutMessage) {
      Stream.flush();
      *OutMessage = strdup(Message.c_str());
    }
    *OutModule = wrap((Module*)nullptr);
    return 1;
  }

  *OutModule = wrap(ModuleOrErr.get().release());
  return 0;
}

/* Reads a module from the specified path, returning via the OutModule parameter
   a module provider which performs lazy deserialization. Returns 0 on success.
   Optionally returns a human-readable error message via OutMessage. */
LLVM37Bool LLVM37GetBitcodeModuleInContext(LLVM37ContextRef ContextRef,
                                       LLVM37MemoryBufferRef MemBuf,
                                       LLVM37ModuleRef *OutM,
                                       char **OutMessage) {
  std::string Message;
  std::unique_ptr<MemoryBuffer> Owner(unwrap(MemBuf));

  ErrorOr<std::unique_ptr<Module>> ModuleOrErr =
      getLazyBitcodeModule(std::move(Owner), *unwrap(ContextRef));
  Owner.release();

  if (std::error_code EC = ModuleOrErr.getError()) {
    *OutM = wrap((Module *)nullptr);
    if (OutMessage)
      *OutMessage = strdup(EC.message().c_str());
    return 1;
  }

  *OutM = wrap(ModuleOrErr.get().release());

  return 0;

}

LLVM37Bool LLVM37GetBitcodeModule(LLVM37MemoryBufferRef MemBuf, LLVM37ModuleRef *OutM,
                              char **OutMessage) {
  return LLVM37GetBitcodeModuleInContext(LLVM37GetGlobalContext(), MemBuf, OutM,
                                       OutMessage);
}

/* Deprecated: Use LLVM37GetBitcodeModuleInContext instead. */
LLVM37Bool LLVM37GetBitcodeModuleProviderInContext(LLVM37ContextRef ContextRef,
                                               LLVM37MemoryBufferRef MemBuf,
                                               LLVM37ModuleProviderRef *OutMP,
                                               char **OutMessage) {
  return LLVM37GetBitcodeModuleInContext(ContextRef, MemBuf,
                                       reinterpret_cast<LLVM37ModuleRef*>(OutMP),
                                       OutMessage);
}

/* Deprecated: Use LLVM37GetBitcodeModule instead. */
LLVM37Bool LLVM37GetBitcodeModuleProvider(LLVM37MemoryBufferRef MemBuf,
                                      LLVM37ModuleProviderRef *OutMP,
                                      char **OutMessage) {
  return LLVM37GetBitcodeModuleProviderInContext(LLVM37GetGlobalContext(), MemBuf,
                                               OutMP, OutMessage);
}
