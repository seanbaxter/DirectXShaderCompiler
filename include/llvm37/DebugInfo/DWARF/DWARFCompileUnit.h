//===-- DWARFCompileUnit.h --------------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_DEBUGINFO_DWARFCOMPILEUNIT_H
#define LLVM37_LIB_DEBUGINFO_DWARFCOMPILEUNIT_H

#include "llvm37/DebugInfo/DWARF/DWARFUnit.h"

namespace llvm37 {

class DWARFCompileUnit : public DWARFUnit {
public:
  DWARFCompileUnit(DWARFContext &Context, const DWARFSection &Section,
                   const DWARFDebugAbbrev *DA, StringRef RS, StringRef SS,
                   StringRef SOS, StringRef AOS, bool LE,
                   const DWARFUnitSectionBase &UnitSection)
      : DWARFUnit(Context, Section, DA, RS, SS, SOS, AOS, LE, UnitSection) {}
  void dump(raw_ostream &OS);
  // VTable anchor.
  ~DWARFCompileUnit() override;
};

}

#endif
