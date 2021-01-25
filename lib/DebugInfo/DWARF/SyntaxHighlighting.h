//===-- SyntaxHighlighting.h ------------------------------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_LIB_DEBUGINFO_SYNTAXHIGHLIGHTING_H
#define LLVM37_LIB_DEBUGINFO_SYNTAXHIGHLIGHTING_H

#include "llvm37/Support/raw_ostream.h"

namespace llvm37 {
namespace dwarf {
namespace syntax {

// Symbolic names for various syntax elements.
enum HighlightColor { Address, String, Tag, Attribute, Enumerator };

/// An RAII object that temporarily switches an output stream to a
/// specific color.
class WithColor {
  llvm37::raw_ostream &OS;

public:
  /// To be used like this: WithColor(OS, syntax::String) << "text";
  WithColor(llvm37::raw_ostream &OS, enum HighlightColor Type);
  ~WithColor();

  llvm37::raw_ostream& get() { return OS; }
  operator llvm37::raw_ostream& () { return OS; }
};
}
}
}

#endif
