//===--- Phases.cpp - Transformations on Driver Types ---------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Driver/Phases.h"
#include "llvm37/Support/ErrorHandling.h"
#include <cassert>

using namespace clang::driver;

const char *phases::getPhaseName(ID Id) {
  switch (Id) {
  case Preprocess: return "preprocessor";
  case Precompile: return "precompiler";
  case Compile: return "compiler";
  case Backend: return "backend";
  case Assemble: return "assembler";
  case Link: return "linker";
  }

  llvm37_unreachable("Invalid phase id.");
}
