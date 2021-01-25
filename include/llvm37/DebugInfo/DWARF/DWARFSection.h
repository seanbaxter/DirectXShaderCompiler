//===-- DWARFSection.h ------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_DEBUGINFO_DWARFSECTION_H
#define LLVM37_LIB_DEBUGINFO_DWARFSECTION_H

#include "llvm37/DebugInfo/DWARF/DWARFRelocMap.h"
#include "llvm37/ADT/StringRef.h"

namespace llvm37 {

struct DWARFSection {
  StringRef Data;
  RelocAddrMap Relocs;
};

}

#endif
