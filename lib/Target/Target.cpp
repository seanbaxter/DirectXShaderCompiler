//===-- Target.cpp --------------------------------------------------------===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file implements the common infrastructure (including C bindings) for 
// libLLVM37Target.a, which implements target information.
//
//===----------------------------------------------------------------------===//

#include "llvm37-c/Target.h"
#include "llvm37-c/Initialization.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Value.h"
#include "llvm37/InitializePasses.h"
#include "llvm37/IR/LegacyPassManager.h"
#include "llvm37/Analysis/TargetLibraryInfo.h"
#include <cstring>

using namespace llvm37;

inline TargetLibraryInfoImpl *unwrap(LLVM37TargetLibraryInfoRef P) {
  return reinterpret_cast<TargetLibraryInfoImpl*>(P);
}

inline LLVM37TargetLibraryInfoRef wrap(const TargetLibraryInfoImpl *P) {
  TargetLibraryInfoImpl *X = const_cast<TargetLibraryInfoImpl*>(P);
  return reinterpret_cast<LLVM37TargetLibraryInfoRef>(X);
}

void llvm37::initializeTarget(PassRegistry &Registry) {
  initializeTargetLibraryInfoWrapperPassPass(Registry);
  initializeTargetTransformInfoWrapperPassPass(Registry);
}

void LLVM37InitializeTarget(LLVM37PassRegistryRef R) {
  initializeTarget(*unwrap(R));
}

LLVM37TargetDataRef LLVM37CreateTargetData(const char *StringRep) {
  return wrap(new DataLayout(StringRep));
}

void LLVM37AddTargetData(LLVM37TargetDataRef TD, LLVM37PassManagerRef PM) {
}

void LLVM37AddTargetLibraryInfo(LLVM37TargetLibraryInfoRef TLI,
                              LLVM37PassManagerRef PM) {
  unwrap(PM)->add(new TargetLibraryInfoWrapperPass(*unwrap(TLI)));
}

char *LLVM37CopyStringRepOfTargetData(LLVM37TargetDataRef TD) {
  std::string StringRep = unwrap(TD)->getStringRepresentation();
  return _strdup(StringRep.c_str()); // HLSL Change strdup to _strdup
}

LLVM37ByteOrdering LLVM37ByteOrder(LLVM37TargetDataRef TD) {
  return unwrap(TD)->isLittleEndian() ? LLVM37LittleEndian : LLVM37BigEndian;
}

unsigned LLVM37PointerSize(LLVM37TargetDataRef TD) {
  return unwrap(TD)->getPointerSize(0);
}

unsigned LLVM37PointerSizeForAS(LLVM37TargetDataRef TD, unsigned AS) {
  return unwrap(TD)->getPointerSize(AS);
}

LLVM37TypeRef LLVM37IntPtrType(LLVM37TargetDataRef TD) {
  return wrap(unwrap(TD)->getIntPtrType(getGlobalContext()));
}

LLVM37TypeRef LLVM37IntPtrTypeForAS(LLVM37TargetDataRef TD, unsigned AS) {
  return wrap(unwrap(TD)->getIntPtrType(getGlobalContext(), AS));
}

LLVM37TypeRef LLVM37IntPtrTypeInContext(LLVM37ContextRef C, LLVM37TargetDataRef TD) {
  return wrap(unwrap(TD)->getIntPtrType(*unwrap(C)));
}

LLVM37TypeRef LLVM37IntPtrTypeForASInContext(LLVM37ContextRef C, LLVM37TargetDataRef TD, unsigned AS) {
  return wrap(unwrap(TD)->getIntPtrType(*unwrap(C), AS));
}

unsigned long long LLVM37SizeOfTypeInBits(LLVM37TargetDataRef TD, LLVM37TypeRef Ty) {
  return unwrap(TD)->getTypeSizeInBits(unwrap(Ty));
}

unsigned long long LLVM37StoreSizeOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty) {
  return unwrap(TD)->getTypeStoreSize(unwrap(Ty));
}

unsigned long long LLVM37ABISizeOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty) {
  return unwrap(TD)->getTypeAllocSize(unwrap(Ty));
}

unsigned LLVM37ABIAlignmentOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty) {
  return unwrap(TD)->getABITypeAlignment(unwrap(Ty));
}

unsigned LLVM37CallFrameAlignmentOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty) {
  return unwrap(TD)->getABITypeAlignment(unwrap(Ty));
}

unsigned LLVM37PreferredAlignmentOfType(LLVM37TargetDataRef TD, LLVM37TypeRef Ty) {
  return unwrap(TD)->getPrefTypeAlignment(unwrap(Ty));
}

unsigned LLVM37PreferredAlignmentOfGlobal(LLVM37TargetDataRef TD,
                                        LLVM37ValueRef GlobalVar) {
  return unwrap(TD)->getPreferredAlignment(unwrap<GlobalVariable>(GlobalVar));
}

unsigned LLVM37ElementAtOffset(LLVM37TargetDataRef TD, LLVM37TypeRef StructTy,
                             unsigned long long Offset) {
  StructType *STy = unwrap<StructType>(StructTy);
  return unwrap(TD)->getStructLayout(STy)->getElementContainingOffset(Offset);
}

unsigned long long LLVM37OffsetOfElement(LLVM37TargetDataRef TD, LLVM37TypeRef StructTy,
                                       unsigned Element) {
  StructType *STy = unwrap<StructType>(StructTy);
  return unwrap(TD)->getStructLayout(STy)->getElementOffset(Element);
}

void LLVM37DisposeTargetData(LLVM37TargetDataRef TD) {
  delete unwrap(TD);
}
