//===- RemoteTarget.cpp - LLVM37 Remote process JIT execution --------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// Implementation of the RemoteTarget class which executes JITed code in a
// separate address range from where it was built.
//
//===----------------------------------------------------------------------===//

#include "RemoteTarget.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/Support/DataTypes.h"
#include "llvm37/Support/Memory.h"
#include <stdlib.h>
#include <string>

using namespace llvm37;

////////////////////////////////////////////////////////////////////////////////
// Simulated remote execution
//
// This implementation will simply move generated code and data to a new memory
// location in the current executable and let it run from there.
////////////////////////////////////////////////////////////////////////////////

bool RemoteTarget::allocateSpace(size_t Size, unsigned Alignment,
                                 uint64_t &Address) {
  sys::MemoryBlock *Prev = Allocations.size() ? &Allocations.back() : nullptr;
  sys::MemoryBlock Mem = sys::Memory::AllocateRWX(Size, Prev, &ErrorMsg);
  if (Mem.base() == nullptr)
    return false;
  if ((uintptr_t)Mem.base() % Alignment) {
    ErrorMsg = "unable to allocate sufficiently aligned memory";
    return false;
  }
  Address = reinterpret_cast<uint64_t>(Mem.base());
  Allocations.push_back(Mem);
  return true;
}

bool RemoteTarget::loadData(uint64_t Address, const void *Data, size_t Size) {
  memcpy ((void*)Address, Data, Size);
  return true;
}

bool RemoteTarget::loadCode(uint64_t Address, const void *Data, size_t Size) {
  memcpy ((void*)Address, Data, Size);
  sys::MemoryBlock Mem((void*)Address, Size);
  sys::Memory::setExecutable(Mem, &ErrorMsg);
  return true;
}

bool RemoteTarget::executeCode(uint64_t Address, int &RetVal) {
  int (*fn)(void) = (int(*)(void))Address;
  RetVal = fn();
  return true;
}

bool RemoteTarget::create() {
  return true;
}

void RemoteTarget::stop() {
  for (unsigned i = 0, e = Allocations.size(); i != e; ++i)
    sys::Memory::ReleaseRWX(Allocations[i]);
}
