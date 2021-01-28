//===--- DeclAccessPair.h - A decl bundled with its path access -*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  This file defines the DeclAccessPair class, which provides an
//  efficient representation of a pair of a NamedDecl* and an
//  AccessSpecifier.  Generally the access specifier gives the
//  natural access of a declaration when named in a class, as
//  defined in C++ [class.access.base]p1.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_AST_DECLACCESSPAIR_H
#define LLVM37_CLANG_AST_DECLACCESSPAIR_H

#include "clang/Basic/Specifiers.h"
#include "llvm37/Support/DataTypes.h"

namespace clang {

class NamedDecl;

/// A POD class for pairing a NamedDecl* with an access specifier.
/// Can be put into unions.
class DeclAccessPair {
  uintptr_t Ptr; // we'd use llvm37::PointerUnion, but it isn't trivial

  enum { Mask = 0x3 };

public:
  static DeclAccessPair make(NamedDecl *D, AccessSpecifier AS) {
    DeclAccessPair p;
    p.set(D, AS);
    return p;
  }

  NamedDecl *getDecl() const {
    return reinterpret_cast<NamedDecl*>(~Mask & Ptr);
  }
  AccessSpecifier getAccess() const {
    return AccessSpecifier(Mask & Ptr);
  }

  void setDecl(NamedDecl *D) {
    set(D, getAccess());
  }
  void setAccess(AccessSpecifier AS) {
    set(getDecl(), AS);
  }
  void set(NamedDecl *D, AccessSpecifier AS) {
    Ptr = uintptr_t(AS) | reinterpret_cast<uintptr_t>(D);
  }

  operator NamedDecl*() const { return getDecl(); }
  NamedDecl *operator->() const { return getDecl(); }
};
}

// Take a moment to tell SmallVector that DeclAccessPair is POD.
namespace llvm37 {
template<typename> struct isPodLike;
template<> struct isPodLike<clang::DeclAccessPair> {
   static const bool value = true;
};
}

#endif
