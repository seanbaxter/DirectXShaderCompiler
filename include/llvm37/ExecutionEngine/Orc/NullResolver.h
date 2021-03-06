//===------ NullResolver.h - Reject symbol lookup requests ------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//   Defines a RuntimeDyld::SymbolResolver subclass that rejects all symbol
// resolution requests, for clients that have no cross-object fixups.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_EXECUTIONENGINE_ORC_NULLRESOLVER_H
#define LLVM37_EXECUTIONENGINE_ORC_NULLRESOLVER_H

#include "llvm37/ExecutionEngine/RuntimeDyld.h"

namespace llvm37 {
namespace orc {

/// SymbolResolver impliementation that rejects all resolution requests.
/// Useful for clients that have no cross-object fixups.
class NullResolver : public RuntimeDyld::SymbolResolver {
public:
  RuntimeDyld::SymbolInfo findSymbol(const std::string &Name) final;

  RuntimeDyld::SymbolInfo
  findSymbolInLogicalDylib(const std::string &Name) final;
};

} // End namespace orc.
} // End namespace llvm37.

#endif // LLVM37_EXECUTIONENGINE_ORC_NULLRESOLVER_H
