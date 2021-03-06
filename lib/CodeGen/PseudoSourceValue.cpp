//===-- llvm37/CodeGen/PseudoSourceValue.cpp ----------------------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the PseudoSourceValue class.
//
//===----------------------------------------------------------------------===//

#include "llvm37/CodeGen/PseudoSourceValue.h"
#include "llvm37/CodeGen/MachineFrameInfo.h"
#include "llvm37/IR/DerivedTypes.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/Support/ErrorHandling.h"
#include "llvm37/Support/ManagedStatic.h"
#include "llvm37/Support/Mutex.h"
#include "llvm37/Support/raw_ostream.h"
#include <map>
using namespace llvm37;

namespace {
struct PSVGlobalsTy {
  // PseudoSourceValues are immutable so don't need locking.
  const PseudoSourceValue PSVs[4];
  sys::Mutex Lock;  // Guards FSValues, but not the values inside it.
  std::map<int, const PseudoSourceValue *> FSValues;

  PSVGlobalsTy() : PSVs() {}
  ~PSVGlobalsTy() {
    for (std::map<int, const PseudoSourceValue *>::iterator
           I = FSValues.begin(), E = FSValues.end(); I != E; ++I) {
      delete I->second;
    }
  }
};

static ManagedStatic<PSVGlobalsTy> PSVGlobals;

}  // anonymous namespace

const PseudoSourceValue *PseudoSourceValue::getStack()
{ return &PSVGlobals->PSVs[0]; }
const PseudoSourceValue *PseudoSourceValue::getGOT()
{ return &PSVGlobals->PSVs[1]; }
const PseudoSourceValue *PseudoSourceValue::getJumpTable()
{ return &PSVGlobals->PSVs[2]; }
const PseudoSourceValue *PseudoSourceValue::getConstantPool()
{ return &PSVGlobals->PSVs[3]; }

static const char *const PSVNames[] = {
  "Stack",
  "GOT",
  "JumpTable",
  "ConstantPool"
};

PseudoSourceValue::PseudoSourceValue(bool isFixed) : isFixed(isFixed) {}

PseudoSourceValue::~PseudoSourceValue() {}

void PseudoSourceValue::printCustom(raw_ostream &O) const {
  O << PSVNames[this - PSVGlobals->PSVs];
}

const PseudoSourceValue *PseudoSourceValue::getFixedStack(int FI) {
  PSVGlobalsTy &PG = *PSVGlobals;
  sys::ScopedLock locked(PG.Lock);
  const PseudoSourceValue *&V = PG.FSValues[FI];
  if (!V)
    V = new FixedStackPseudoSourceValue(FI);
  return V;
}

bool PseudoSourceValue::isConstant(const MachineFrameInfo *) const {
  if (this == getStack())
    return false;
  if (this == getGOT() ||
      this == getConstantPool() ||
      this == getJumpTable())
    return true;
  llvm37_unreachable("Unknown PseudoSourceValue!");
}

bool PseudoSourceValue::isAliased(const MachineFrameInfo *MFI) const {
  if (this == getStack() ||
      this == getGOT() ||
      this == getConstantPool() ||
      this == getJumpTable())
    return false;
  llvm37_unreachable("Unknown PseudoSourceValue!");
}

bool PseudoSourceValue::mayAlias(const MachineFrameInfo *MFI) const {
  if (this == getGOT() ||
      this == getConstantPool() ||
      this == getJumpTable())
    return false;
  return true;
}

bool FixedStackPseudoSourceValue::isConstant(const MachineFrameInfo *MFI) const{
  return MFI && MFI->isImmutableObjectIndex(FI);
}

bool FixedStackPseudoSourceValue::isAliased(const MachineFrameInfo *MFI) const {
  if (!MFI)
    return true;
  return MFI->isAliasedObjectIndex(FI);
}

bool FixedStackPseudoSourceValue::mayAlias(const MachineFrameInfo *MFI) const {
  if (!MFI)
    return true;
  // Spill slots will not alias any LLVM37 IR value.
  return !MFI->isSpillSlotObjectIndex(FI);
}

void FixedStackPseudoSourceValue::printCustom(raw_ostream &OS) const {
  OS << "FixedStack" << FI;
}
