//===- EnumDumper.cpp -------------------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "EnumDumper.h"

#include "BuiltinDumper.h"
#include "LinePrinter.h"
#include "llvm37-pdbdump.h"

#include "llvm37/DebugInfo/PDB/PDBSymbolData.h"
#include "llvm37/DebugInfo/PDB/PDBSymbolTypeBuiltin.h"
#include "llvm37/DebugInfo/PDB/PDBSymbolTypeEnum.h"

using namespace llvm37;

EnumDumper::EnumDumper(LinePrinter &P) : PDBSymDumper(true), Printer(P) {}

void EnumDumper::start(const PDBSymbolTypeEnum &Symbol) {
  WithColor(Printer, PDB_ColorItem::Keyword).get() << "enum ";
  WithColor(Printer, PDB_ColorItem::Type).get() << Symbol.getName();
  if (!opts::NoEnumDefs) {
    auto BuiltinType = Symbol.getUnderlyingType();
    if (BuiltinType->getBuiltinType() != PDB_BuiltinType::Int ||
        BuiltinType->getLength() != 4) {
      Printer << " : ";
      BuiltinDumper Dumper(Printer);
      Dumper.start(*BuiltinType);
    }
    Printer << " {";
    Printer.Indent();
    auto EnumValues = Symbol.findAllChildren<PDBSymbolData>();
    while (auto EnumValue = EnumValues->getNext()) {
      if (EnumValue->getDataKind() != PDB_DataKind::Constant)
        continue;
      Printer.NewLine();
      WithColor(Printer, PDB_ColorItem::Identifier).get()
          << EnumValue->getName();
      Printer << " = ";
      WithColor(Printer, PDB_ColorItem::LiteralValue).get()
          << EnumValue->getValue();
    }
    Printer.Unindent();
    Printer.NewLine();
    Printer << "}";
  }
}
