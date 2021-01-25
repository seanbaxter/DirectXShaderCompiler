///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilAnnotateWithVirtualRegister.cpp                                       //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Annotates the llvm instructions with a virtual register number to be used //
// during PIX debugging.                                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <memory>

#include "dxc/DXIL/DxilModule.h"
#include "dxc/DxilPIXPasses/DxilPIXPasses.h"
#include "dxc/DxilPIXPasses/DxilPIXVirtualRegisters.h"
#include "dxc/Support/Global.h"
#include "llvm37/ADT/DenseMap.h"
#include "llvm37/ADT/StringRef.h"
#include "llvm37/IR/Constant.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/DebugInfo.h"
#include "llvm37/IR/DebugInfoMetadata.h"
#include "llvm37/IR/IRBuilder.h"
#include "llvm37/IR/InstIterator.h"
#include "llvm37/IR/Instruction.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/Module.h"
#include "llvm37/IR/ModuleSlotTracker.h"
#include "llvm37/IR/Type.h"
#include "llvm37/Pass.h"
#include "llvm37/Support/Casting.h"
#include "llvm37/Support/raw_ostream.h"

#define DEBUG_TYPE "dxil-annotate-with-virtual-regs"

uint32_t CountStructMembers(llvm37::Type const *pType) {
  uint32_t Count = 0;

  if (auto *ST = llvm37::dyn_cast<llvm37::StructType>(pType)) {
    for (auto &El : ST->elements()) {
      Count += CountStructMembers(El);
    }
  } else if (auto *AT = llvm37::dyn_cast<llvm37::ArrayType>(pType)) {
    Count = CountStructMembers(AT->getArrayElementType()) *
            AT->getArrayNumElements();
  } else {
    Count = 1;
  }
  return Count;
}

namespace {
using namespace pix_dxil;

static bool IsInstrumentableFundamentalType(llvm37::Type *pAllocaTy) {
  return
    pAllocaTy->isFloatingPointTy() || pAllocaTy->isIntegerTy();
}

class DxilAnnotateWithVirtualRegister : public llvm37::ModulePass {
public:
  static char ID;
  DxilAnnotateWithVirtualRegister() : llvm37::ModulePass(ID) {}

  bool runOnModule(llvm37::Module &M) override;

private:
  void AnnotateValues(llvm37::Instruction *pI);
  void AnnotateStore(llvm37::Instruction *pI);
  bool IsAllocaRegisterWrite(llvm37::Value *V, llvm37::AllocaInst **pAI,
                             llvm37::Value **pIdx);
  void AnnotateAlloca(llvm37::AllocaInst *pAlloca);
  void AnnotateGeneric(llvm37::Instruction *pI);
  void AssignNewDxilRegister(llvm37::Instruction *pI);
  void PrintSingleRegister(llvm37::Instruction* pI, uint32_t Register);
  void AssignNewAllocaRegister(llvm37::AllocaInst* pAlloca, std::uint32_t C);
  void PrintAllocaMember(llvm37::AllocaInst* pAlloca, uint32_t Base, uint32_t Offset);

  hlsl::DxilModule* m_DM;
  std::uint32_t m_uVReg;
  std::unique_ptr<llvm37::ModuleSlotTracker> m_MST;
  void Init(llvm37::Module &M) {
    m_DM = &M.GetOrCreateDxilModule();
    m_uVReg = 0;
    m_MST.reset(new llvm37::ModuleSlotTracker(&M));
    m_MST->incorporateFunction(*m_DM->GetEntryFunction());
  }
};

char DxilAnnotateWithVirtualRegister::ID = 0;

bool DxilAnnotateWithVirtualRegister::runOnModule(llvm37::Module &M) {
  Init(M);
  if (m_DM == nullptr) {
    return false;
  }
  unsigned int Major = 0;
  unsigned int Minor = 0;
  m_DM->GetDxilVersion(Major, Minor);
  if (Major < 6 || (Major == 6 && Minor <= 4)) {
    m_DM->SetValidatorVersion(1, 4);
  }

  std::uint32_t InstNum = 0;
  for (llvm37::Instruction &I : llvm37::inst_range(m_DM->GetEntryFunction())) {
    if (!llvm37::isa<llvm37::DbgDeclareInst>(&I)) {
      pix_dxil::PixDxilInstNum::AddMD(M.getContext(), &I, InstNum++);
    }
  }

  if (OSOverride != nullptr) {
    *OSOverride << "\nInstructionCount:" << InstNum << "\n";
  }

  if (OSOverride != nullptr) {
    *OSOverride << "\nEnd - instruction ID to line\n";
  }

  if (OSOverride != nullptr) {
    *OSOverride << "\nBegin - dxil values to virtual register mapping\n";
  }

  for (llvm37::Instruction &I : llvm37::inst_range(m_DM->GetEntryFunction())) {
    AnnotateValues(&I);
  }

  for (llvm37::Instruction &I : llvm37::inst_range(m_DM->GetEntryFunction())) {
    AnnotateStore(&I);
  }

  if (OSOverride != nullptr) {
    *OSOverride << "\nEnd - dxil values to virtual register mapping\n";
  }

  m_DM = nullptr;
  return m_uVReg > 0;
}

void DxilAnnotateWithVirtualRegister::AnnotateValues(llvm37::Instruction *pI) {
  if (auto* pAlloca = llvm37::dyn_cast<llvm37::AllocaInst>(pI)) {
    AnnotateAlloca(pAlloca);
  } else if (!pI->getType()->isPointerTy()) {
    AnnotateGeneric(pI);
  } else if (!pI->getType()->isVoidTy()) {
    AnnotateGeneric(pI);
  }
}

void DxilAnnotateWithVirtualRegister::AnnotateStore(llvm37::Instruction *pI) {
  auto *pSt = llvm37::dyn_cast<llvm37::StoreInst>(pI);
  if (pSt == nullptr) {
    return;
  }

  llvm37::AllocaInst *Alloca;
  llvm37::Value *Index;
  if (!IsAllocaRegisterWrite(pSt->getPointerOperand(), &Alloca, &Index)) {
    return;
  }

  llvm37::MDNode *AllocaReg = Alloca->getMetadata(PixAllocaReg::MDName);
  if (AllocaReg == nullptr) {
    return;
  }

  PixAllocaRegWrite::AddMD(m_DM->GetCtx(), pSt, AllocaReg, Index);
}

static uint32_t GetStructOffset(
  llvm37::GetElementPtrInst* pGEP,
  uint32_t& GEPOperandIndex,
  llvm37::Type* pElementType)
{
  if (IsInstrumentableFundamentalType(pElementType)) {
    return 0;
  }
  else if (auto* pArray = llvm37::dyn_cast<llvm37::ArrayType>(pElementType))
  {
    // 1D-array example:
    //
    // When referring to the zeroth member of the array in this struct:
    // struct smallPayload {
    //   uint32_t Array[2];
    // };
    // getelementptr inbounds% struct.smallPayload, % struct.smallPayload*% p,
    // i32 0, i32 0, i32 0 The zeros above are:
    //  -The zeroth element in the array pointed to (so, the actual struct)
    //  -The zeroth element in the struct (which is the array)
    //  -The zeroth element in that array

    auto* pArrayIndex =
      llvm37::dyn_cast<llvm37::ConstantInt>(pGEP->getOperand(GEPOperandIndex++));

    if (pArrayIndex == nullptr) {
      return 0;
    }

    uint32_t ArrayIndex = pArrayIndex->getLimitedValue();
    auto pArrayElementType = pArray->getArrayElementType();
    uint32_t MemberIndex = ArrayIndex * CountStructMembers(pArrayElementType);
    return MemberIndex + GetStructOffset(pGEP, GEPOperandIndex, pArrayElementType);
  }
  else if (auto* pStruct = llvm37::dyn_cast<llvm37::StructType>(pElementType))
  {
    DXASSERT(GEPOperandIndex < pGEP->getNumOperands(), "Unexpectedly read too many GetElementPtrInst operands");

    auto* pMemberIndex =
      llvm37::dyn_cast<llvm37::ConstantInt>(pGEP->getOperand(GEPOperandIndex++));

    if (pMemberIndex == nullptr) {
      return 0;
    }

    uint32_t MemberIndex = pMemberIndex->getLimitedValue();

    uint32_t MemberOffset = 0;
    for (uint32_t i = 0; i < MemberIndex; ++i)
    {
      MemberOffset += CountStructMembers(pStruct->getElementType(i));
    }

    return MemberOffset +
      GetStructOffset(pGEP, GEPOperandIndex, pStruct->getElementType(MemberIndex));
  }
  else
  {
    return 0;
  }
}


bool DxilAnnotateWithVirtualRegister::IsAllocaRegisterWrite(
    llvm37::Value *V, llvm37::AllocaInst **pAI, llvm37::Value **pIdx) {
  llvm37::IRBuilder<> B(m_DM->GetCtx());

  *pAI = nullptr;
  *pIdx = nullptr;

  if (auto *pGEP = llvm37::dyn_cast<llvm37::GetElementPtrInst>(V)) {
    uint32_t precedingMemberCount = 0;
    auto *Alloca = llvm37::dyn_cast<llvm37::AllocaInst>(pGEP->getPointerOperand());
    if (Alloca == nullptr) {
      // In the case of vector types (floatN, matrixNxM), the pointer operand will actually
      // point to another element pointer instruction. But this isn't a recursive thing-
      // we only need to check these two levels.
      if (auto* pPointerGEP = llvm37::dyn_cast<llvm37::GetElementPtrInst>(pGEP->getPointerOperand())) {
        Alloca =
            llvm37::dyn_cast<llvm37::AllocaInst>(pPointerGEP->getPointerOperand());
        if (Alloca == nullptr) {
          return false;
        }
        // And of course the member we're after might not be at the beginning of the struct:
        auto* pStructType  = llvm37::dyn_cast<llvm37::StructType>(pPointerGEP->getPointerOperandType()->getPointerElementType());
        auto* pStructMember = llvm37::dyn_cast<llvm37::ConstantInt>(pPointerGEP->getOperand(2));
        uint64_t memberIndex = pStructMember->getLimitedValue();
        for(uint64_t i = 0; i < memberIndex; ++i)
        {
          precedingMemberCount += CountStructMembers(pStructType->getStructElementType(i));
        }
      }
      else
      {
        return false;
      }
    }


    // Deref pointer type to get struct type:
    llvm37::Type *pStructType = pGEP->getPointerOperandType();
    pStructType = pStructType->getContainedType(0);

    // The 1th operand is an index into the array of the above type. In DXIL derived from HLSL,
    // we always expect this to be 0 (since llvm structs are only used for single-valued
    // objects in HLSL, such as the amplification-to-mesh or TraceRays payloads).
    uint32_t GEPOperandIndex = 1;
    auto *pBaseArrayIndex =
        llvm37::dyn_cast<llvm37::ConstantInt>(pGEP->getOperand(GEPOperandIndex++));
    DXASSERT_LOCALVAR(pBaseArrayIndex, pBaseArrayIndex != nullptr,
                      "null base array index pointer");
    DXASSERT_LOCALVAR(pBaseArrayIndex, pBaseArrayIndex->getLimitedValue() == 0,
                      "unexpected >0 array index");

    // From here on, the indices always come in groups: first, the type 
    // referenced in the current struct. If that type is an (n-dimensional)
    // array, then there follow n indices.


    auto offset = GetStructOffset(
      pGEP,
      GEPOperandIndex,
      pStructType);

    llvm37::Value* IndexValue = B.getInt32(offset + precedingMemberCount);

    if (IndexValue != nullptr)
    {
      *pAI = Alloca;
      *pIdx = IndexValue;
      return true;
    }
    return false;
  }

  if (auto *pAlloca = llvm37::dyn_cast<llvm37::AllocaInst>(V)) {
    llvm37::Type *pAllocaTy = pAlloca->getType()->getElementType();
    if (!IsInstrumentableFundamentalType(pAllocaTy)) {
      return false;
    }

    *pAI = pAlloca;
    *pIdx = B.getInt32(0);
    return true;
  }

  return false;
}

void DxilAnnotateWithVirtualRegister::AnnotateAlloca(
    llvm37::AllocaInst *pAlloca) {
  llvm37::Type *pAllocaTy = pAlloca->getType()->getElementType();
  if (IsInstrumentableFundamentalType(pAllocaTy)) {
    AssignNewAllocaRegister(pAlloca, 1);
  } else if (auto *AT = llvm37::dyn_cast<llvm37::ArrayType>(pAllocaTy)) {
    AssignNewAllocaRegister(pAlloca, AT->getNumElements());
  } else if (auto *ST = llvm37::dyn_cast<llvm37::StructType>(pAllocaTy)) {
    AssignNewAllocaRegister(pAlloca, CountStructMembers(ST));
  } else {
    DXASSERT_ARGS(false, "Unhandled alloca kind: %d", pAllocaTy->getTypeID());
  }
}

void DxilAnnotateWithVirtualRegister::AnnotateGeneric(llvm37::Instruction *pI) {
  if (auto *GEP = llvm37::dyn_cast<llvm37::GetElementPtrInst>(pI)) {
    // https://llvm.org/docs/LangRef.html#getelementptr-instruction
    DXASSERT(!GEP->getOperand(1)->getType()->isVectorTy(),
             "struct vectors not supported");
    llvm37::AllocaInst *StructAlloca =
        llvm37::dyn_cast<llvm37::AllocaInst>(GEP->getOperand(0));
    if (StructAlloca != nullptr) {
      // This is the case of a pointer to a struct member. 
      // We treat it as an alias of the actual member in the alloca.
      std::uint32_t baseStructRegNum = 0;
      std::uint32_t regSize = 0;
      if (pix_dxil::PixAllocaReg::FromInst(StructAlloca, &baseStructRegNum, & regSize)) {
        llvm37::ConstantInt *OffsetAsInt =
            llvm37::dyn_cast<llvm37::ConstantInt>(GEP->getOperand(2));
        if (OffsetAsInt != nullptr)
        {
          std::uint32_t Offset = static_cast<std::uint32_t>(
            OffsetAsInt->getValue().getLimitedValue());
          DXASSERT(Offset < regSize,
            "Structure member offset out of expected range");
          PixDxilReg::AddMD(m_DM->GetCtx(), pI, baseStructRegNum + Offset);
        }
      }
    }
  } else {
    if (!IsInstrumentableFundamentalType(pI->getType())) {
      return;
    }
    AssignNewDxilRegister(pI);
  }
}

void DxilAnnotateWithVirtualRegister::AssignNewDxilRegister(
    llvm37::Instruction *pI) {
  PixDxilReg::AddMD(m_DM->GetCtx(), pI, m_uVReg);
  PrintSingleRegister(pI, m_uVReg);
  m_uVReg++;
}

void DxilAnnotateWithVirtualRegister::AssignNewAllocaRegister(
    llvm37::AllocaInst *pAlloca, std::uint32_t C) {
  PixAllocaReg::AddMD(m_DM->GetCtx(), pAlloca, m_uVReg, C);
  PrintAllocaMember(pAlloca, m_uVReg, C);
  m_uVReg += C;
}

void DxilAnnotateWithVirtualRegister::PrintSingleRegister(
    llvm37::Instruction* pI, uint32_t Register) {
  if (OSOverride != nullptr) {
    static constexpr bool DontPrintType = false;
    pI->printAsOperand(*OSOverride, DontPrintType, *m_MST.get());
    *OSOverride << " dxil " << Register << "\n";
  }
}

void DxilAnnotateWithVirtualRegister::PrintAllocaMember(llvm37::AllocaInst* pAlloca,
                                                   uint32_t Base,
                                                   uint32_t Offset) {
  if (OSOverride != nullptr) {
    static constexpr bool DontPrintType = false;
    pAlloca->printAsOperand(*OSOverride, DontPrintType, *m_MST.get());
    *OSOverride << " alloca " << Base << " " << Offset << "\n";
  }
}

} // namespace

using namespace llvm37;

INITIALIZE_PASS(DxilAnnotateWithVirtualRegister, DEBUG_TYPE,
                "Annotates each instruction in the DXIL module with a virtual "
                "register number",
                false, false)

ModulePass *llvm37::createDxilAnnotateWithVirtualRegisterPass() {
  return new DxilAnnotateWithVirtualRegister();
}
