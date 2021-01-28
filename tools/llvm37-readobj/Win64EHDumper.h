//===- Win64EHDumper.h - Win64 EH Printing ----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37_READOBJ_WIN64EHDUMPER_H
#define LLVM37_TOOLS_LLVM37_READOBJ_WIN64EHDUMPER_H

#include "StreamWriter.h"
#include "llvm37/Support/Win64EH.h"

namespace llvm37 {
namespace object {
class COFFObjectFile;
class SymbolRef;
struct coff_section;
}

namespace Win64EH {
class Dumper {
  StreamWriter &SW;
  raw_ostream &OS;

public:
  typedef std::error_code (*SymbolResolver)(const object::coff_section *,
                                            uint64_t, object::SymbolRef &,
                                            void *);

  struct Context {
    const object::COFFObjectFile &COFF;
    SymbolResolver ResolveSymbol;
    void *UserData;

    Context(const object::COFFObjectFile &COFF, SymbolResolver Resolver,
            void *UserData)
      : COFF(COFF), ResolveSymbol(Resolver), UserData(UserData) {}
  };

private:
  void printRuntimeFunctionEntry(const Context &Ctx,
                                 const object::coff_section *Section,
                                 uint64_t SectionOffset,
                                 const RuntimeFunction &RF);
  void printUnwindCode(const UnwindInfo& UI, ArrayRef<UnwindCode> UC);
  void printUnwindInfo(const Context &Ctx, const object::coff_section *Section,
                       off_t Offset, const UnwindInfo &UI);
  void printRuntimeFunction(const Context &Ctx,
                            const object::coff_section *Section,
                            uint64_t SectionOffset, const RuntimeFunction &RF);

public:
  Dumper(StreamWriter &SW) : SW(SW), OS(SW.getOStream()) {}

  void printData(const Context &Ctx);
};
}
}

#endif
