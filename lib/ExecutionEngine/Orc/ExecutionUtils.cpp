//===---- ExecutionUtils.cpp - Utilities for executing functions in Orc ---===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/ExecutionEngine/Orc/ExecutionUtils.h"

#include "llvm37/IR/Constants.h"
#include "llvm37/IR/Function.h"
#include "llvm37/IR/GlobalVariable.h"
#include "llvm37/IR/Module.h"

namespace llvm37 {
namespace orc {

CtorDtorIterator::CtorDtorIterator(const GlobalVariable *GV, bool End)
  : InitList(
      GV ? dyn_cast_or_null<ConstantArray>(GV->getInitializer()) : nullptr),
    I((InitList && End) ? InitList->getNumOperands() : 0) {
}

bool CtorDtorIterator::operator==(const CtorDtorIterator &Other) const {
  assert(InitList == Other.InitList && "Incomparable iterators.");
  return I == Other.I;
}

bool CtorDtorIterator::operator!=(const CtorDtorIterator &Other) const {
  return !(*this == Other);
}

CtorDtorIterator& CtorDtorIterator::operator++() {
  ++I;
  return *this;
}

CtorDtorIterator CtorDtorIterator::operator++(int) {
  CtorDtorIterator Temp = *this;
  ++I;
  return Temp;
}

CtorDtorIterator::Element CtorDtorIterator::operator*() const {
  ConstantStruct *CS = dyn_cast<ConstantStruct>(InitList->getOperand(I));
  assert(CS && "Unrecognized type in llvm37.global_ctors/llvm37.global_dtors");

  Constant *FuncC = CS->getOperand(1);
  Function *Func = nullptr;

  // Extract function pointer, pulling off any casts.
  while (FuncC) {
    if (Function *F = dyn_cast_or_null<Function>(FuncC)) {
      Func = F;
      break;
    } else if (ConstantExpr *CE = dyn_cast_or_null<ConstantExpr>(FuncC)) {
      if (CE->isCast())
        FuncC = dyn_cast_or_null<ConstantExpr>(CE->getOperand(0));
      else
        break;
    } else {
      // This isn't anything we recognize. Bail out with Func left set to null.
      break;
    }
  }

  ConstantInt *Priority = dyn_cast<ConstantInt>(CS->getOperand(0));
  Value *Data = CS->getOperand(2);
  return Element(Priority->getZExtValue(), Func, Data);
}

iterator_range<CtorDtorIterator> getConstructors(const Module &M) {
  const GlobalVariable *CtorsList = M.getNamedGlobal("llvm.global_ctors");
  return make_range(CtorDtorIterator(CtorsList, false),
                    CtorDtorIterator(CtorsList, true));
}

iterator_range<CtorDtorIterator> getDestructors(const Module &M) {
  const GlobalVariable *DtorsList = M.getNamedGlobal("llvm.global_dtors");
  return make_range(CtorDtorIterator(DtorsList, false),
                    CtorDtorIterator(DtorsList, true));
}

void LocalCXXRuntimeOverrides::runDestructors() {
  auto& CXXDestructorDataPairs = DSOHandleOverride;
  for (auto &P : CXXDestructorDataPairs)
    P.first(P.second);
  CXXDestructorDataPairs.clear();
}

int LocalCXXRuntimeOverrides::CXAAtExitOverride(DestructorPtr Destructor,
                                                void *Arg, void *DSOHandle) {
  auto& CXXDestructorDataPairs =
    *reinterpret_cast<CXXDestructorDataPairList*>(DSOHandle);
  CXXDestructorDataPairs.push_back(std::make_pair(Destructor, Arg));
  return 0;
}

} // End namespace orc.
} // End namespace llvm37.
