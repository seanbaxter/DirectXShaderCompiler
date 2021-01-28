//===--- LangStandards.cpp - Language Standard Definitions ----------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/LangStandard.h"
#include "llvm37/ADT/StringSwitch.h"
#include "llvm37/Support/ErrorHandling.h"
using namespace clang;
using namespace clang::frontend;

#define LANGSTANDARD(id, name, desc, features) \
  static const LangStandard Lang_##id = { name, desc, features };
#include "clang/Frontend/LangStandards.def"

const LangStandard &LangStandard::getLangStandardForKind(Kind K) {
  switch (K) {
  case lang_unspecified:
    llvm37::report_fatal_error("getLangStandardForKind() on unspecified kind");
#define LANGSTANDARD(id, name, desc, features) \
    case lang_##id: return Lang_##id;
#include "clang/Frontend/LangStandards.def"
  }
  llvm37_unreachable("Invalid language kind!");
}

const LangStandard *LangStandard::getLangStandardForName(StringRef Name) {
  Kind K = llvm37::StringSwitch<Kind>(Name)
#define LANGSTANDARD(id, name, desc, features) \
    .Case(name, lang_##id)
#include "clang/Frontend/LangStandards.def"
    .Default(lang_unspecified);
  if (K == lang_unspecified)
    return nullptr;

  return &getLangStandardForKind(K);
}


