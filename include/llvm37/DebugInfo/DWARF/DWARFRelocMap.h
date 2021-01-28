//===-- DWARFRelocMap.h -----------------------------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_DEBUGINFO_DWARFRELOCMAP_H
#define LLVM37_LIB_DEBUGINFO_DWARFRELOCMAP_H

#include "llvm37/ADT/DenseMap.h"

namespace llvm37 {

typedef DenseMap<uint64_t, std::pair<uint8_t, int64_t> > RelocAddrMap;

} // namespace llvm37

#endif

