//===-- llvm37/CodeGen/AddressPool.cpp - Dwarf Debug Framework ---*- C++ -*--===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "AddressPool.h"
#include "llvm37/CodeGen/AsmPrinter.h"
#include "llvm37/MC/MCStreamer.h"
#include "llvm37/Target/TargetLoweringObjectFile.h"

using namespace llvm37;

class MCExpr;

unsigned AddressPool::getIndex(const MCSymbol *Sym, bool TLS) {
  HasBeenUsed = true;
  auto IterBool =
      Pool.insert(std::make_pair(Sym, AddressPoolEntry(Pool.size(), TLS)));
  return IterBool.first->second.Number;
}

// Emit addresses into the section given.
void AddressPool::emit(AsmPrinter &Asm, MCSection *AddrSection) {
  if (Pool.empty())
    return;

  // Start the dwarf addr section.
  Asm.OutStreamer->SwitchSection(AddrSection);

  // Order the address pool entries by ID
  SmallVector<const MCExpr *, 64> Entries(Pool.size());

  for (const auto &I : Pool)
    Entries[I.second.Number] =
        I.second.TLS
            ? Asm.getObjFileLowering().getDebugThreadLocalSymbol(I.first)
            : MCSymbolRefExpr::create(I.first, Asm.OutContext);

  for (const MCExpr *Entry : Entries)
    Asm.OutStreamer->EmitValue(Entry, Asm.getDataLayout().getPointerSize());
}
