//===--- StringSet.h - The LLVM37 Compiler Driver -----------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open
// Source License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  StringSet - A set-like wrapper for the StringMap.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_ADT_STRINGSET_H
#define LLVM37_ADT_STRINGSET_H

#include "llvm37/ADT/StringMap.h"

namespace llvm37 {

  /// StringSet - A wrapper for StringMap that provides set-like functionality.
  template <class AllocatorTy = llvm37::MallocAllocator>
  class StringSet : public llvm37::StringMap<char, AllocatorTy> {
    typedef llvm37::StringMap<char, AllocatorTy> base;
  public:

    std::pair<typename base::iterator, bool> insert(StringRef Key) {
      assert(!Key.empty());
      return base::insert(std::make_pair(Key, '\0'));
    }
  };
}

#endif // LLVM37_ADT_STRINGSET_H
