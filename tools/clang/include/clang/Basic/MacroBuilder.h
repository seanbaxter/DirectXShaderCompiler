//===--- MacroBuilder.h - CPP Macro building utility ------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
///
/// \file
/// \brief Defines the clang::MacroBuilder utility class.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM37_CLANG_BASIC_MACROBUILDER_H
#define LLVM37_CLANG_BASIC_MACROBUILDER_H

#include "clang/Basic/LLVM.h"
#include "llvm37/ADT/Twine.h"
#include "llvm37/Support/raw_ostream.h"

namespace clang {

class MacroBuilder {
  raw_ostream &Out;
public:
  MacroBuilder(raw_ostream &Output) : Out(Output) {}

  /// Append a \#define line for macro of the form "\#define Name Value\n".
  void defineMacro(const Twine &Name, const Twine &Value = "1") {
    Out << "#define " << Name << ' ' << Value << '\n';
  }

  /// Append a \#undef line for Name.  Name should be of the form XXX
  /// and we emit "\#undef XXX".
  void undefineMacro(const Twine &Name) {
    Out << "#undef " << Name << '\n';
  }

  /// Directly append Str and a newline to the underlying buffer.
  void append(const Twine &Str) {
    Out << Str << '\n';
  }
};

}  // end namespace clang

#endif
