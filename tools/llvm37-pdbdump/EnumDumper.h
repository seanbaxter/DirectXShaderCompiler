//===- EnumDumper.h - -------------------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_TOOLS_LLVM37PDBDUMP_ENUMDUMPER_H
#define LLVM37_TOOLS_LLVM37PDBDUMP_ENUMDUMPER_H

#include "llvm37/DebugInfo/PDB/PDBSymDumper.h"

namespace llvm37 {

class LinePrinter;

class EnumDumper : public PDBSymDumper {
public:
  EnumDumper(LinePrinter &P);

  void start(const PDBSymbolTypeEnum &Symbol);

private:
  LinePrinter &Printer;
};
}

#endif
