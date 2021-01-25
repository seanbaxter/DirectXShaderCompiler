//===- VariableDumper.h - PDBSymDumper implementation for types -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVMPDBDUMP_VARIABLEDUMPER_H
#define LLVM37_TOOLS_LLVMPDBDUMP_VARIABLEDUMPER_H

#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"
#include "llvm37/ADT/StringRef.h"

namespace llvm37 {

class LinePrinter;

class VariableDumper : public PDBSymDumper {
public:
  VariableDumper(LinePrinter &P);

  void start(const PDBSymbolData &Var);

  void dump(const PDBSymbolTypeBuiltin &Symbol) override;
  void dump(const PDBSymbolTypeEnum &Symbol) override;
  void dump(const PDBSymbolTypeFunctionSig &Symbol) override;
  void dump(const PDBSymbolTypePointer &Symbol) override;
  void dump(const PDBSymbolTypeTypedef &Symbol) override;
  void dump(const PDBSymbolTypeUDT &Symbol) override;

private:
  void dumpSymbolTypeAndName(const PDBSymbol &Type, StringRef Name);
  bool tryDumpFunctionPointer(const PDBSymbol &Type, StringRef Name);

  LinePrinter &Printer;
};
}

#endif
