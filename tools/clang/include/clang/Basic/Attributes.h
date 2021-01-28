//===--- Attributes.h - Attributes header -----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_BASIC_ATTRIBUTES_H
#define LLVM37_CLANG_BASIC_ATTRIBUTES_H

#include "clang/Basic/LangOptions.h"
#include "llvm37/ADT/Triple.h"

namespace clang {

class IdentifierInfo;

enum class AttrSyntax {
  /// Is the identifier known as a GNU-style attribute?
  GNU,
  /// Is the identifier known as a __declspec-style attribute?
  Declspec,
  // Is the identifier known as a C++-style attribute?
  CXX,
  // Is the identifier known as a pragma attribute?
  Pragma
};

/// \brief Return the version number associated with the attribute if we
/// recognize and implement the attribute specified by the given information.
int hasAttribute(AttrSyntax Syntax, const IdentifierInfo *Scope,
                 const IdentifierInfo *Attr, const llvm37::Triple &T,
                 const LangOptions &LangOpts);

} // end namespace clang

#endif // LLVM37_CLANG_BASIC_ATTRIBUTES_H
