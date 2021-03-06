///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilPIXVirtualRegisters.cpp                                               //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Declares functions for dealing with the virtual register annotations in   //
// DXIL instructions.                                                        //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <cstdint>

namespace llvm37 {
class AllocaInst;
class Instruction;
class LLVM37Context;
class MDNode;
class StoreInst;
class Value;
}  // namespace llvm37

namespace pix_dxil {
namespace PixDxilInstNum {
static constexpr char MDName[] = "pix-dxil-inst-num";
static constexpr uint32_t ID = 3;

void AddMD(llvm37::LLVM37Context &Ctx, llvm37::Instruction *pI, std::uint32_t InstNum);
bool FromInst(llvm37::Instruction *pI, std::uint32_t *pInstNum);
}  // namespace PixDxilInstNum

namespace PixDxilReg {
static constexpr char MDName[] = "pix-dxil-reg";
static constexpr uint32_t ID = 0;

void AddMD(llvm37::LLVM37Context &Ctx, llvm37::Instruction *pI, std::uint32_t RegNum);
bool FromInst(llvm37::Instruction *pI, std::uint32_t *pRegNum);
}  // namespace PixDxilReg

namespace PixAllocaReg {
static constexpr char MDName[] = "pix-alloca-reg";
static constexpr uint32_t ID = 1;

void AddMD(llvm37::LLVM37Context &Ctx, llvm37::AllocaInst *pAlloca, std::uint32_t RegNum, std::uint32_t Count);
bool FromInst(llvm37::AllocaInst *pAlloca, std::uint32_t *pRegBase, std::uint32_t *pRegSize);
}  // namespace PixAllocaReg

namespace PixAllocaRegWrite {
static constexpr char MDName[] = "pix-alloca-reg-write";
static constexpr uint32_t ID = 2;
void AddMD(llvm37::LLVM37Context &Ctx, llvm37::StoreInst *pSt, llvm37::MDNode *pAllocaReg, llvm37::Value *Index);
bool FromInst(llvm37::StoreInst *pI, std::uint32_t *pRegBase, std::uint32_t *pRegSize, llvm37::Value **pIndex);
}  // namespace PixAllocaRegWrite
}  // namespace pix_dxil