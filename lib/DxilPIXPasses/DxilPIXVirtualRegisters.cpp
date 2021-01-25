///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilPIXVirtualRegisters.cpp                                               //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Defines functions for dealing with the virtual register annotations in    //
// DXIL instructions.                                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "dxc/DxilPIXPasses/DxilPIXVirtualRegisters.h"

#include "dxc/Support/Global.h"
#include "llvm37/IR/Constant.h"
#include "llvm37/IR/Constants.h"
#include "llvm37/IR/IRBuilder.h"
#include "llvm37/IR/InstIterator.h"
#include "llvm37/IR/Instruction.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/Metadata.h"
#include "llvm37/IR/Type.h"

void pix_dxil::PixDxilInstNum::AddMD(llvm37::LLVMContext &Ctx,
                                     llvm37::Instruction *pI,
                                     std::uint32_t InstNum) {
  llvm37::IRBuilder<> B(Ctx);
  pI->setMetadata(
      llvm37::StringRef(MDName),
      llvm37::MDNode::get(Ctx,
                        {llvm37::ConstantAsMetadata::get(B.getInt32(ID)),
                         llvm37::ConstantAsMetadata::get(B.getInt32(InstNum))}));
}

bool pix_dxil::PixDxilInstNum::FromInst(llvm37::Instruction *pI,
                                        std::uint32_t *pInstNum) {
  *pInstNum = 0;

  auto *mdNodes = pI->getMetadata(MDName);

  if (mdNodes == nullptr) {
    return false;
  }

  if (mdNodes->getNumOperands() != 2) {
    return false;
  }

  auto *mdID =
      llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(mdNodes->getOperand(0));
  if (mdID == nullptr || mdID->getLimitedValue() != ID) {
    return false;
  }

  auto *mdInstNum =
      llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(mdNodes->getOperand(1));
  if (mdInstNum == nullptr) {
    return false;
  }

  *pInstNum = mdInstNum->getLimitedValue();
  return true;
}

void pix_dxil::PixDxilReg::AddMD(llvm37::LLVMContext &Ctx, llvm37::Instruction *pI,
                                 std::uint32_t RegNum) {
  llvm37::IRBuilder<> B(Ctx);
  pI->setMetadata(
      llvm37::StringRef(MDName),
      llvm37::MDNode::get(Ctx,
                        {llvm37::ConstantAsMetadata::get(B.getInt32(ID)),
                         llvm37::ConstantAsMetadata::get(B.getInt32(RegNum))}));
}

bool pix_dxil::PixDxilReg::FromInst(llvm37::Instruction *pI,
                                    std::uint32_t *pRegNum) {
  *pRegNum = 0;

  auto *mdNodes = pI->getMetadata(MDName);

  if (mdNodes == nullptr) {
    return false;
  }

  if (mdNodes->getNumOperands() != 2) {
    return false;
  }

  auto *mdID =
      llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(mdNodes->getOperand(0));
  if (mdID == nullptr || mdID->getLimitedValue() != ID) {
    return false;
  }

  auto *mdRegNum =
      llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(mdNodes->getOperand(1));
  if (mdRegNum == nullptr) {
    return false;
  }

  *pRegNum = mdRegNum->getLimitedValue();
  return true;
}

static bool ParsePixAllocaReg(llvm37::MDNode *MD, std::uint32_t *RegNum,
                              std::uint32_t *Count) {
  if (MD->getNumOperands() != 3) {
    return false;
  }

  auto *mdID = llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(MD->getOperand(0));
  if (mdID == nullptr ||
      mdID->getLimitedValue() != pix_dxil::PixAllocaReg::ID) {
    return false;
  }

  auto *mdRegNum =
      llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(MD->getOperand(1));
  auto *mdCount =
      llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(MD->getOperand(2));

  if (mdRegNum == nullptr || mdCount == nullptr) {
    return false;
  }

  *RegNum = mdRegNum->getLimitedValue();
  *Count = mdCount->getLimitedValue();
  return true;
}

void pix_dxil::PixAllocaReg::AddMD(llvm37::LLVMContext &Ctx,
                                   llvm37::AllocaInst *pAlloca,
                                   std::uint32_t RegNum, std::uint32_t Count) {
  llvm37::IRBuilder<> B(Ctx);
  pAlloca->setMetadata(
      llvm37::StringRef(MDName),
      llvm37::MDNode::get(Ctx,
                        {llvm37::ConstantAsMetadata::get(B.getInt32(ID)),
                         llvm37::ConstantAsMetadata::get(B.getInt32(RegNum)),
                         llvm37::ConstantAsMetadata::get(B.getInt32(Count))}));
}

bool pix_dxil::PixAllocaReg::FromInst(llvm37::AllocaInst *pAlloca,
                                      std::uint32_t *pRegBase,
                                      std::uint32_t *pRegSize) {
  *pRegBase = 0;
  *pRegSize = 0;

  auto *mdNodes = pAlloca->getMetadata(MDName);
  if (mdNodes == nullptr) {
    return false;
  }

  return ParsePixAllocaReg(mdNodes, pRegBase, pRegSize);
}

namespace pix_dxil {
namespace PixAllocaRegWrite {
static constexpr uint32_t IndexIsConst = 1;
static constexpr uint32_t IndexIsPixInst = 2;
} // namespace PixAllocaRegWrite
} // namespace pix_dxil

void pix_dxil::PixAllocaRegWrite::AddMD(llvm37::LLVMContext &Ctx,
                                        llvm37::StoreInst *pSt,
                                        llvm37::MDNode *pAllocaReg,
                                        llvm37::Value *Index) {
  llvm37::IRBuilder<> B(Ctx);
  if (auto *C = llvm37::dyn_cast<llvm37::ConstantInt>(Index)) {
    pSt->setMetadata(
        llvm37::StringRef(MDName),
        llvm37::MDNode::get(
            Ctx, {llvm37::ConstantAsMetadata::get(B.getInt32(ID)), pAllocaReg,
                  llvm37::ConstantAsMetadata::get(B.getInt32(IndexIsConst)),
                  llvm37::ConstantAsMetadata::get(C)}));
  }

  if (auto *I = llvm37::dyn_cast<llvm37::Instruction>(Index)) {
    std::uint32_t InstNum;
    if (!PixDxilInstNum::FromInst(I, &InstNum)) {
      return;
    }
    pSt->setMetadata(
        llvm37::StringRef(MDName),
        llvm37::MDNode::get(
            Ctx, {llvm37::ConstantAsMetadata::get(B.getInt32(ID)), pAllocaReg,
                  llvm37::ConstantAsMetadata::get(B.getInt32(IndexIsPixInst)),
                  llvm37::ConstantAsMetadata::get(B.getInt32(InstNum))}));
  }
}

bool pix_dxil::PixAllocaRegWrite::FromInst(llvm37::StoreInst *pI,
                                           std::uint32_t *pRegBase,
                                           std::uint32_t *pRegSize,
                                           llvm37::Value **pIndex) {
  *pRegBase = 0;
  *pRegSize = 0;
  *pIndex = nullptr;

  auto *mdNodes = pI->getMetadata(MDName);
  if (mdNodes == nullptr || mdNodes->getNumOperands() != 4) {
    return false;
  }

  auto *mdID =
      llvm37::mdconst::dyn_extract<llvm37::ConstantInt>(mdNodes->getOperand(0));
  if (mdID == nullptr || mdID->getLimitedValue() != ID) {
    return false;
  }

  auto *mdAllocaReg = llvm37::dyn_cast<llvm37::MDNode>(mdNodes->getOperand(1));
  if (mdAllocaReg == nullptr ||
      !ParsePixAllocaReg(mdAllocaReg, pRegBase, pRegSize)) {
    return false;
  }

  auto *mdIndexType =
      llvm37::dyn_cast<llvm37::ConstantAsMetadata>(mdNodes->getOperand(2));
  if (mdIndexType == nullptr) {
    return false;
  }

  auto *cIndexType = llvm37::dyn_cast<llvm37::ConstantInt>(mdIndexType->getValue());
  if (cIndexType == nullptr) {
    return false;
  }

  auto *mdIndex =
      llvm37::dyn_cast<llvm37::ConstantAsMetadata>(mdNodes->getOperand(3));
  if (mdIndex == nullptr) {
    return false;
  }

  auto *cIndex = llvm37::dyn_cast<llvm37::ConstantInt>(mdIndex->getValue());
  if (cIndex == nullptr) {
    return false;
  }

  switch (cIndexType->getLimitedValue()) {
  default:
    return false;

  case IndexIsConst: {
    *pIndex = cIndex;
    return true;
  }

  case IndexIsPixInst: {
    for (llvm37::Instruction &I :
         llvm37::inst_range(pI->getParent()->getParent())) {
      uint32_t InstNum;
      if (PixDxilInstNum::FromInst(&I, &InstNum)) {
        *pIndex = &I;
        if (InstNum == cIndex->getLimitedValue()) {
          return true;
        }
      }
    }
    return false;
  }
  }

  return false;
}
