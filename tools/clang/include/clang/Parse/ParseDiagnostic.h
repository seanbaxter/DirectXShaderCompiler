//===--- DiagnosticParse.h - Diagnostics for libparse -----------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_PARSE_PARSEDIAGNOSTIC_H
#define LLVM37_CLANG_PARSE_PARSEDIAGNOSTIC_H

#include "clang/Basic/Diagnostic.h"

namespace clang {
  namespace diag {
    enum {
#define DIAG(ENUM,FLAGS,DEFAULT_MAPPING,DESC,GROUP,\
             SFINAE,NOWERROR,SHOWINSYSHEADER,CATEGORY) ENUM,
#define PARSESTART
#include "clang/Basic/DiagnosticParseKinds.inc"
#undef DIAG
      NUM_BUILTIN_PARSE_DIAGNOSTICS
    };
  }  // end namespace diag
}  // end namespace clang

#endif
