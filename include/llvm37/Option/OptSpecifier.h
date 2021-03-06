//===--- OptSpecifier.h - Option Specifiers ---------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_OPTION_OPTSPECIFIER_H
#define LLVM37_OPTION_OPTSPECIFIER_H

#include "llvm37/Support/Compiler.h"

namespace llvm37 {
namespace opt {
  class Option;

  /// OptSpecifier - Wrapper class for abstracting references to option IDs.
  class OptSpecifier {
    unsigned ID;

  private:
    explicit OptSpecifier(bool) = delete;

  public:
    OptSpecifier() : ID(0) {}
    /*implicit*/ OptSpecifier(unsigned ID) : ID(ID) {}
    /*implicit*/ OptSpecifier(const Option *Opt);

    bool isValid() const { return ID != 0; }

    unsigned getID() const { return ID; }

    bool operator==(OptSpecifier Opt) const { return ID == Opt.getID(); }
    bool operator!=(OptSpecifier Opt) const { return !(*this == Opt); }
  };
}
}

#endif
