//===- llvm37/Support/Atomic.h - Atomic Operations -----------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the llvm37::sys atomic operations.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_ATOMIC_H
#define LLVM37_SUPPORT_ATOMIC_H

#include "llvm37/Support/DataTypes.h"

namespace llvm37 {
  namespace sys {
    void MemoryFence();

#ifdef _MSC_VER
    typedef long cas_flag;
#else
    typedef uint32_t cas_flag;
#endif
    cas_flag CompareAndSwap(volatile cas_flag* ptr,
                            cas_flag new_value,
                            cas_flag old_value);
    cas_flag AtomicIncrement(volatile cas_flag* ptr);
    cas_flag AtomicDecrement(volatile cas_flag* ptr);
    cas_flag AtomicAdd(volatile cas_flag* ptr, cas_flag val);
    cas_flag AtomicMul(volatile cas_flag* ptr, cas_flag val);
    cas_flag AtomicDiv(volatile cas_flag* ptr, cas_flag val);
  }
}

#endif
