//===---------- NullResolver.cpp - Reject symbol lookup requests ----------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ExecutionEngine/Orc/NullResolver.h"

#include "llvm37/Support/ErrorHandling.h"

namespace llvm37 {
namespace orc {

RuntimeDyld::SymbolInfo NullResolver::findSymbol(const std::string &Name) {
  llvm37_unreachable("Unexpected cross-object symbol reference");
}

RuntimeDyld::SymbolInfo
NullResolver::findSymbolInLogicalDylib(const std::string &Name) {
  llvm37_unreachable("Unexpected cross-object symbol reference");
}

} // End namespace orc.
} // End namespace llvm37.
