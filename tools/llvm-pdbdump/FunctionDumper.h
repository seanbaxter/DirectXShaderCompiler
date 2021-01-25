//===- FunctionDumper.h --------------------------------------- *- C++ --*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVMPDBDUMP_FUNCTIONDUMPER_H
#define LLVM37_TOOLS_LLVMPDBDUMP_FUNCTIONDUMPER_H

#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

namespace llvm37 {

class LinePrinter;

class FunctionDumper : public PDBSymDumper {
public:
  FunctionDumper(LinePrinter &P);

  enum class PointerType { None, Pointer, Reference };

  void start(const PDBSymbolTypeFunctionSig &Symbol, const char *Name,
             PointerType Pointer);
  void start(const PDBSymbolFunc &Symbol, PointerType Pointer);

  void dump(const PDBSymbolTypeArray &Symbol) override;
  void dump(const PDBSymbolTypeBuiltin &Symbol) override;
  void dump(const PDBSymbolTypeEnum &Symbol) override;
  void dump(const PDBSymbolTypeFunctionArg &Symbol) override;
  void dump(const PDBSymbolTypePointer &Symbol) override;
  void dump(const PDBSymbolTypeTypedef &Symbol) override;
  void dump(const PDBSymbolTypeUDT &Symbol) override;

private:
  LinePrinter &Printer;
};
}

#endif
