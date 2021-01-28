//===- llvm37/Support/CBindingWrapph.h - C Interface Wrapping -----*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file declares the wrapping macros for the C interface.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_SUPPORT_CBINDINGWRAPPING_H
#define LLVM37_SUPPORT_CBINDINGWRAPPING_H

#include "llvm37/Support/Casting.h"

#define DEFINE_SIMPLE_CONVERSION_FUNCTIONS(ty, ref)     \
  inline ty *unwrap(ref P) {                            \
    return reinterpret_cast<ty*>(P);                    \
  }                                                     \
                                                        \
  inline ref wrap(const ty *P) {                        \
    return reinterpret_cast<ref>(const_cast<ty*>(P));   \
  }

#define DEFINE_ISA_CONVERSION_FUNCTIONS(ty, ref)        \
  DEFINE_SIMPLE_CONVERSION_FUNCTIONS(ty, ref)           \
                                                        \
  template<typename T>                                  \
  inline T *unwrap(ref P) {                             \
    return cast<T>(unwrap(P));                          \
  }

#define DEFINE_STDCXX_CONVERSION_FUNCTIONS(ty, ref)     \
  DEFINE_SIMPLE_CONVERSION_FUNCTIONS(ty, ref)           \
                                                        \
  template<typename T>                                  \
  inline T *unwrap(ref P) {                             \
    T *Q = (T*)unwrap(P);                               \
    assert(Q && "Invalid cast!");                       \
    return Q;                                           \
  }

#endif
