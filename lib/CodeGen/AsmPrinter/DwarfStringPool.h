//===-- llvm37/CodeGen/DwarfStringPool.h - Dwarf Debug Framework -*- C++ -*--===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_CODEGEN_ASMPRINTER_DWARFSTRINGPOOL_H
#define LLVM37_LIB_CODEGEN_ASMPRINTER_DWARFSTRINGPOOL_H

#include "llvm37/ADT/StringMap.h"
#include "llvm37/CodeGen/DwarfStringPoolEntry.h"
#include "llvm37/Support/Allocator.h"
#include <utility>

namespace llvm37 {

class AsmPrinter;
class MCSymbol;
class MCSection;
class StringRef;

// Collection of strings for this unit and assorted symbols.
// A String->Symbol mapping of strings used by indirect
// references.
class DwarfStringPool {
  typedef DwarfStringPoolEntry EntryTy;
  StringMap<EntryTy, BumpPtrAllocator &> Pool;
  StringRef Prefix;
  unsigned NumBytes = 0;
  bool ShouldCreateSymbols;

public:
  typedef DwarfStringPoolEntryRef EntryRef;

  DwarfStringPool(BumpPtrAllocator &A, AsmPrinter &Asm, StringRef Prefix);

  void emit(AsmPrinter &Asm, MCSection *StrSection,
            MCSection *OffsetSection = nullptr);

  bool empty() const { return Pool.empty(); }

  /// Get a reference to an entry in the string pool.
  EntryRef getEntry(AsmPrinter &Asm, StringRef Str);
};
}
#endif
