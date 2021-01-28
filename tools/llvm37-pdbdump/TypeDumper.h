//===- TypeDumper.h - PDBSymDumper implementation for types *- C++ ------*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37PDBDUMP_TYPEDUMPER_H
#define LLVM37_TOOLS_LLVM37PDBDUMP_TYPEDUMPER_H

#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

namespace llvm37 {

class LinePrinter;

class TypeDumper : public PDBSymDumper {
public:
  TypeDumper(LinePrinter &P);

  void start(const PDBSymbolExe &Exe);

  void dump(const PDBSymbolTypeEnum &Symbol) override;
  void dump(const PDBSymbolTypeTypedef &Symbol) override;
  void dump(const PDBSymbolTypeUDT &Symbol) override;

private:
  LinePrinter &Printer;
};
}

#endif
