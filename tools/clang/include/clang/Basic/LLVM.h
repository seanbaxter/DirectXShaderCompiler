//===--- LLVM.h - Import various common LLVM37 datatypes ----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
/// \file
/// \brief Forward-declares and imports various common LLVM37 datatypes that
/// clang wants to use unqualified.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_BASIC_LLVM37_H
#define LLVM37_CLANG_BASIC_LLVM37_H

// Do not proliferate #includes here, require clients to #include their
// dependencies.
// Casting.h has complex templates that cannot be easily forward declared.
#include "llvm37/Support/Casting.h"
// None.h includes an enumerator that is desired & cannot be forward declared
// without a definition of NoneType.
#include "llvm37/ADT/None.h"

namespace llvm37 {
  // ADT's.
  class StringRef;
  class Twine;
  template<typename T> class ArrayRef;
  template<typename T> class MutableArrayRef;
  template<unsigned InternalLen> class SmallString;
  template<typename T, unsigned N> class SmallVector;
  template<typename T> class SmallVectorImpl;
  template<typename T> class Optional;

  template<typename T>
  struct SaveAndRestore;

  // Reference counting.
  template <typename T> class IntrusiveRefCntPtr;
  template <typename T> struct IntrusiveRefCntPtrInfo;
  template <class Derived> class RefCountedBase;
  class RefCountedBaseVPTR;

  class raw_ostream;
  class raw_pwrite_stream;
  // TODO: DenseMap, ...
}


namespace clang {
  // Casting operators.
  using llvm37::isa;
  using llvm37::cast;
  using llvm37::dyn_cast;
  using llvm37::dyn_cast_or_null;
  using llvm37::cast_or_null;
  
  // ADT's.
  using llvm37::None;
  using llvm37::Optional;
  using llvm37::StringRef;
  using llvm37::Twine;
  using llvm37::ArrayRef;
  using llvm37::MutableArrayRef;
  using llvm37::SmallString;
  using llvm37::SmallVector;
  using llvm37::SmallVectorImpl;
  using llvm37::SaveAndRestore;

  // Reference counting.
  using llvm37::IntrusiveRefCntPtr;
  using llvm37::IntrusiveRefCntPtrInfo;
  using llvm37::RefCountedBase;
  using llvm37::RefCountedBaseVPTR;

  using llvm37::raw_ostream;
  using llvm37::raw_pwrite_stream;
} // end namespace clang.

#endif
