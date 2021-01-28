//===-- StringSaver.cpp ---------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Support/StringSaver.h"

using namespace llvm37;

const char *StringSaver::saveImpl(StringRef S) {
  char *P = Alloc.Allocate<char>(S.size() + 1);
  memcpy(P, S.data(), S.size());
  P[S.size()] = '\0';
  return P;
}
