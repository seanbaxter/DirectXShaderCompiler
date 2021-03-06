//===--- Capacity.h - Generic computation of ADT memory use -----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines the capacity function that computes the amount of
// memory used by an ADT.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_CAPACITY_H
#define LLVM37_SUPPORT_CAPACITY_H

#include <cstddef>

namespace llvm37 {

template <typename T>
static inline size_t capacity_in_bytes(const T &x) {
  // This default definition of capacity should work for things like std::vector
  // and friends.  More specialized versions will work for others.
  return x.capacity() * sizeof(typename T::value_type);
}

} // end namespace llvm37

#endif

