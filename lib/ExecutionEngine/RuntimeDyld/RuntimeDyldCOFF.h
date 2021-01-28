//===-- RuntimeDyldCOFF.h - Run-time dynamic linker for MC-JIT ---*- C++ -*-==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// COFF support for MC-JIT runtime dynamic linker.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_RUNTIME_DYLD_COFF_H
#define LLVM37_RUNTIME_DYLD_COFF_H

#include "RuntimeDyldImpl.h"
#include "llvm37/ADT/DenseMap.h"

#define DEBUG_TYPE "dyld"

using namespace llvm37;

namespace llvm37 {

// Common base class for COFF dynamic linker support.
// Concrete subclasses for each target can be found in ./Targets.
class RuntimeDyldCOFF : public RuntimeDyldImpl {

public:
  std::unique_ptr<RuntimeDyld::LoadedObjectInfo>
  loadObject(const object::ObjectFile &Obj) override;
  bool isCompatibleFile(const object::ObjectFile &Obj) const override;

  static std::unique_ptr<RuntimeDyldCOFF>
  create(Triple::ArchType Arch, RuntimeDyld::MemoryManager &MemMgr,
         RuntimeDyld::SymbolResolver &Resolver);

protected:
  RuntimeDyldCOFF(RuntimeDyld::MemoryManager &MemMgr,
                  RuntimeDyld::SymbolResolver &Resolver)
    : RuntimeDyldImpl(MemMgr, Resolver) {}
  uint64_t getSymbolOffset(const SymbolRef &Sym);
};

} // end namespace llvm37

#undef DEBUG_TYPE

#endif
