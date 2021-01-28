//===- ExternalSymbolDumper.h --------------------------------- *- C++ --*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37PDBDUMP_EXTERNALSYMBOLDUMPER_H
#define LLVM37_TOOLS_LLVM37PDBDUMP_EXTERNALSYMBOLDUMPER_H

#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

namespace llvm37 {

class LinePrinter;

class ExternalSymbolDumper : public PDBSymDumper {
public:
  ExternalSymbolDumper(LinePrinter &P);

  void start(const PDBSymbolExe &Symbol);

  void dump(const PDBSymbolPublicSymbol &Symbol) override;

private:
  LinePrinter &Printer;
};
}

#endif
