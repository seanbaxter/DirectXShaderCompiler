//===- SymbolSize.h ---------------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_OBJECT_SYMBOLSIZE_H
#define LLVM37_OBJECT_SYMBOLSIZE_H

#include "llvm37/Object/ObjectFile.h"

namespace llvm37 {
namespace object {
std::vector<std::pair<SymbolRef, uint64_t>>
computeSymbolSizes(const ObjectFile &O);
}
} // namespace llvm37

#endif
