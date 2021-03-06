//===-- llvm37/GlobalObject.h - Class to represent global objects -*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This represents an independent object. That is, a function or a global
// variable, but not an alias.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_IR_GLOBALOBJECT_H
#define LLVM37_IR_GLOBALOBJECT_H

#include "llvm37/IR/Constant.h"
#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/GlobalValue.h"

namespace llvm37 {
class Comdat;
class Module;

class GlobalObject : public GlobalValue {
  GlobalObject(const GlobalObject &) = delete;

protected:
  GlobalObject(PointerType *Ty, ValueTy VTy, Use *Ops, unsigned NumOps,
               LinkageTypes Linkage, const Twine &Name)
      : GlobalValue(Ty, VTy, Ops, NumOps, Linkage, Name), ObjComdat(nullptr) {
    setGlobalValueSubClassData(0);
  }

  std::string Section;     // Section to emit this into, empty means default
  Comdat *ObjComdat;
  static const unsigned AlignmentBits = 5;
  static const unsigned GlobalObjectSubClassDataBits =
      GlobalValueSubClassDataBits - AlignmentBits;

private:
  static const unsigned AlignmentMask = (1 << AlignmentBits) - 1;

public:
  unsigned getAlignment() const {
    unsigned Data = getGlobalValueSubClassData();
    unsigned AlignmentData = Data & AlignmentMask;
    return (1u << AlignmentData) >> 1;
  }
  void setAlignment(unsigned Align);

  unsigned getGlobalObjectSubClassData() const;
  void setGlobalObjectSubClassData(unsigned Val);

  bool hasSection() const { return !StringRef(getSection()).empty(); }
  const char *getSection() const { return Section.c_str(); }
  void setSection(StringRef S);

  bool hasComdat() const { return getComdat() != nullptr; }
  const Comdat *getComdat() const { return ObjComdat; }
  Comdat *getComdat() { return ObjComdat; }
  void setComdat(Comdat *C) { ObjComdat = C; }

  void copyAttributesFrom(const GlobalValue *Src) override;

  // Methods for support type inquiry through isa, cast, and dyn_cast:
  static inline bool classof(const Value *V) {
    return V->getValueID() == Value::FunctionVal ||
           V->getValueID() == Value::GlobalVariableVal;
  }
};

} // End llvm37 namespace

#endif
