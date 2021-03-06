//===- MemoryLocation.cpp - Memory location descriptions -------------------==//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "llvm37/Analysis/MemoryLocation.h"
#include "llvm37/Analysis/TargetLibraryInfo.h"
#include "llvm37/IR/BasicBlock.h"
#include "llvm37/IR/DataLayout.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/LLVMContext.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/Type.h"
using namespace llvm37;

MemoryLocation MemoryLocation::get(const LoadInst *LI) {
  AAMDNodes AATags;
  LI->getAAMetadata(AATags);
  const auto &DL = LI->getModule()->getDataLayout();

  return MemoryLocation(LI->getPointerOperand(),
                        DL.getTypeStoreSize(LI->getType()), AATags);
}

MemoryLocation MemoryLocation::get(const StoreInst *SI) {
  AAMDNodes AATags;
  SI->getAAMetadata(AATags);
  const auto &DL = SI->getModule()->getDataLayout();

  return MemoryLocation(SI->getPointerOperand(),
                        DL.getTypeStoreSize(SI->getValueOperand()->getType()),
                        AATags);
}

MemoryLocation MemoryLocation::get(const VAArgInst *VI) {
  AAMDNodes AATags;
  VI->getAAMetadata(AATags);

  return MemoryLocation(VI->getPointerOperand(), UnknownSize, AATags);
}

MemoryLocation MemoryLocation::get(const AtomicCmpXchgInst *CXI) {
  AAMDNodes AATags;
  CXI->getAAMetadata(AATags);
  const auto &DL = CXI->getModule()->getDataLayout();

  return MemoryLocation(
      CXI->getPointerOperand(),
      DL.getTypeStoreSize(CXI->getCompareOperand()->getType()), AATags);
}

MemoryLocation MemoryLocation::get(const AtomicRMWInst *RMWI) {
  AAMDNodes AATags;
  RMWI->getAAMetadata(AATags);
  const auto &DL = RMWI->getModule()->getDataLayout();

  return MemoryLocation(RMWI->getPointerOperand(),
                        DL.getTypeStoreSize(RMWI->getValOperand()->getType()),
                        AATags);
}

MemoryLocation MemoryLocation::getForSource(const MemTransferInst *MTI) {
  uint64_t Size = UnknownSize;
  if (ConstantInt *C = dyn_cast<ConstantInt>(MTI->getLength()))
    Size = C->getValue().getZExtValue();

  // memcpy/memmove can have AA tags. For memcpy, they apply
  // to both the source and the destination.
  AAMDNodes AATags;
  MTI->getAAMetadata(AATags);

  return MemoryLocation(MTI->getRawSource(), Size, AATags);
}

MemoryLocation MemoryLocation::getForDest(const MemIntrinsic *MTI) {
  uint64_t Size = UnknownSize;
  if (ConstantInt *C = dyn_cast<ConstantInt>(MTI->getLength()))
    Size = C->getValue().getZExtValue();

  // memcpy/memmove can have AA tags. For memcpy, they apply
  // to both the source and the destination.
  AAMDNodes AATags;
  MTI->getAAMetadata(AATags);

  return MemoryLocation(MTI->getRawDest(), Size, AATags);
}

// FIXME: This code is duplicated with BasicAliasAnalysis and should be hoisted
// to some common utility location.
static bool isMemsetPattern16(const Function *MS,
                              const TargetLibraryInfo &TLI) {
  if (TLI.has(LibFunc::memset_pattern16) &&
      MS->getName() == "memset_pattern16") {
    FunctionType *MemsetType = MS->getFunctionType();
    if (!MemsetType->isVarArg() && MemsetType->getNumParams() == 3 &&
        isa<PointerType>(MemsetType->getParamType(0)) &&
        isa<PointerType>(MemsetType->getParamType(1)) &&
        isa<IntegerType>(MemsetType->getParamType(2)))
      return true;
  }

  return false;
}

MemoryLocation MemoryLocation::getForArgument(ImmutableCallSite CS,
                                              unsigned ArgIdx,
                                              const TargetLibraryInfo &TLI) {
  AAMDNodes AATags;
  CS->getAAMetadata(AATags);
  const Value *Arg = CS.getArgument(ArgIdx);

  // We may be able to produce an exact size for known intrinsics.
  if (const IntrinsicInst *II = dyn_cast<IntrinsicInst>(CS.getInstruction())) {
    const DataLayout &DL = II->getModule()->getDataLayout();
    (void)DL; // HLSL Change - unreferenced local variable

    switch (II->getIntrinsicID()) {
    default:
      break;
    case Intrinsic::memset:
    case Intrinsic::memcpy:
    case Intrinsic::memmove:
      assert((ArgIdx == 0 || ArgIdx == 1) &&
             "Invalid argument index for memory intrinsic");
      if (ConstantInt *LenCI = dyn_cast<ConstantInt>(II->getArgOperand(2)))
        return MemoryLocation(Arg, LenCI->getZExtValue(), AATags);
      break;

    case Intrinsic::lifetime_start:
    case Intrinsic::lifetime_end:
    case Intrinsic::invariant_start:
      assert(ArgIdx == 1 && "Invalid argument index");
      return MemoryLocation(
          Arg, cast<ConstantInt>(II->getArgOperand(0))->getZExtValue(), AATags);

    case Intrinsic::invariant_end:
      assert(ArgIdx == 2 && "Invalid argument index");
      return MemoryLocation(
          Arg, cast<ConstantInt>(II->getArgOperand(1))->getZExtValue(), AATags);

#if 0 // HLSL Change - remove platform intrinsics
    case Intrinsic::arm_neon_vld1:
      assert(ArgIdx == 0 && "Invalid argument index");
      // LLVM37's vld1 and vst1 intrinsics currently only support a single
      // vector register.
      return MemoryLocation(Arg, DL.getTypeStoreSize(II->getType()), AATags);

    case Intrinsic::arm_neon_vst1:
      assert(ArgIdx == 0 && "Invalid argument index");
      return MemoryLocation(
          Arg, DL.getTypeStoreSize(II->getArgOperand(1)->getType()), AATags);
#endif // HLSL Change - remove platform intrinsics
    }
  }

  // We can bound the aliasing properties of memset_pattern16 just as we can
  // for memcpy/memset.  This is particularly important because the
  // LoopIdiomRecognizer likes to turn loops into calls to memset_pattern16
  // whenever possible.
  if (CS.getCalledFunction() &&
      isMemsetPattern16(CS.getCalledFunction(), TLI)) {
    assert((ArgIdx == 0 || ArgIdx == 1) &&
           "Invalid argument index for memset_pattern16");
    if (ArgIdx == 1)
      return MemoryLocation(Arg, 16, AATags);
    if (const ConstantInt *LenCI = dyn_cast<ConstantInt>(CS.getArgument(2)))
      return MemoryLocation(Arg, LenCI->getZExtValue(), AATags);
  }
  // FIXME: Handle memset_pattern4 and memset_pattern8 also.

  return MemoryLocation(CS.getArgument(ArgIdx), UnknownSize, AATags);
}
