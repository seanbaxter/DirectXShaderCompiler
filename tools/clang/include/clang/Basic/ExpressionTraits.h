//===- ExpressionTraits.h - C++ Expression Traits Support Enums -*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines enumerations for expression traits intrinsics.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_BASIC_EXPRESSIONTRAITS_H
#define LLVM37_CLANG_BASIC_EXPRESSIONTRAITS_H

namespace clang {

  enum ExpressionTrait {
    ET_IsLValueExpr,
    ET_IsRValueExpr
  };
}

#endif
