//===--- CapturedStmt.h - Types for CapturedStmts ---------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//


#ifndef LLVM37_CLANG_BASIC_CAPTUREDSTMT_H
#define LLVM37_CLANG_BASIC_CAPTUREDSTMT_H

namespace clang {

/// \brief The different kinds of captured statement.
enum CapturedRegionKind {
  CR_Default,
  CR_OpenMP
};

} // end namespace clang

#endif // LLVM37_CLANG_BASIC_CAPTUREDSTMT_H
