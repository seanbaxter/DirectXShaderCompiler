//===-- llvm37/IR/Comdat.h - Comdat definitions -------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
/// @file
/// This file contains the declaration of the Comdat class, which represents a
/// single COMDAT in LLVM37.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_IR_COMDAT_H
#define LLVM37_IR_COMDAT_H

#include "llvm37/ADT/StringRef.h"
#include "llvm37/Support/Compiler.h"

namespace llvm37 {

class raw_ostream;
template <typename ValueTy> class StringMapEntry;

// This is a Name X SelectionKind pair. The reason for having this be an
// independent object instead of just adding the name and the SelectionKind
// to a GlobalObject is that it is invalid to have two Comdats with the same
// name but different SelectionKind. This structure makes that unrepresentable.
class Comdat {
public:
  enum SelectionKind {
    Any,          ///< The linker may choose any COMDAT.
    ExactMatch,   ///< The data referenced by the COMDAT must be the same.
    Largest,      ///< The linker will choose the largest COMDAT.
    NoDuplicates, ///< No other Module may specify this COMDAT.
    SameSize,     ///< The data referenced by the COMDAT must be the same size.
  };

  Comdat(Comdat &&C);
  SelectionKind getSelectionKind() const { return SK; }
  void setSelectionKind(SelectionKind Val) { SK = Val; }
  StringRef getName() const;
  void print(raw_ostream &OS) const;
  void dump() const;

private:
  friend class Module;
  Comdat();
  Comdat(SelectionKind SK, StringMapEntry<Comdat> *Name);
  Comdat(const Comdat &) = delete;

  // Points to the map in Module.
  StringMapEntry<Comdat> *Name;
  SelectionKind SK;
};

inline raw_ostream &operator<<(raw_ostream &OS, const Comdat &C) {
  C.print(OS);
  return OS;
}

} // end llvm37 namespace

#endif
