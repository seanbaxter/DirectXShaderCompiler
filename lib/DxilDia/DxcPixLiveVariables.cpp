///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxcPixLiveVariables.cpp                                                   //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Defines the mapping between instructions and the set of live variables    //
// for it.                                                                   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "dxc/Support/WinIncludes.h"

#include "DxcPixDxilDebugInfo.h"
#include "DxcPixLiveVariables.h"
#include "DxcPixLiveVariables_FragmentIterator.h" 
#include "DxilDiaSession.h"

#include "dxc/Support/Global.h"
#include "llvm37/ADT/SmallVector.h"
#include "llvm37/IR/DebugInfo.h"
#include "llvm37/IR/DebugInfoMetadata.h"
#include "llvm37/IR/Dominators.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/Intrinsics.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/Module.h"

#include <unordered_map>

// ValidateDbgDeclare ensures that all of the bits in
// [FragmentSizeInBits, FragmentOffsetInBits) are currently
// not assigned to a dxil alloca register -- i.e., it
// tries to find overlapping alloca registers -- which should
// never happen -- to report the issue.
void ValidateDbgDeclare(
    dxil_debug_info::VariableInfo *VarInfo,
    unsigned FragmentSizeInBits,
    unsigned FragmentOffsetInBits
)
{
#ifndef NDEBUG
  for (unsigned i = 0; i < FragmentSizeInBits; ++i)
  {
    const unsigned BitNum = FragmentOffsetInBits + i;

    VarInfo->m_DbgDeclareValidation.resize(
        std::max<unsigned>(VarInfo->m_DbgDeclareValidation.size(),
                           BitNum + 1));
    assert(!VarInfo->m_DbgDeclareValidation[BitNum]);
    VarInfo->m_DbgDeclareValidation[BitNum] = true;
  }
#endif  // !NDEBUG
}

struct dxil_debug_info::LiveVariables::Impl
{
  using VariableInfoMap = std::unordered_map<
      llvm37::DIVariable *,
      std::unique_ptr<VariableInfo>>;

  using LiveVarsMap =
      std::unordered_map<llvm37::DIScope*, VariableInfoMap>;

  IMalloc *m_pMalloc;
  DxcPixDxilDebugInfo *m_pDxilDebugInfo;
  llvm37::Module *m_pModule;
  LiveVarsMap m_LiveVarsDbgDeclare;

  void Init(
      IMalloc *pMalloc,
      DxcPixDxilDebugInfo *pDxilDebugInfo,
      llvm37::Module *pModule);

  void Init_DbgDeclare(llvm37::DbgDeclareInst *DbgDeclare);

  VariableInfo *AssignValueToOffset(
      VariableInfoMap *VarInfoMap,
      llvm37::DIVariable *Var,
      llvm37::Value *Address,
      unsigned FragmentIndex,
      unsigned FragmentOffsetInBits);
};

void dxil_debug_info::LiveVariables::Impl::Init(
    IMalloc *pMalloc,
    DxcPixDxilDebugInfo *pDxilDebugInfo,
    llvm37::Module *pModule)
{
  m_pMalloc = pMalloc;
  m_pDxilDebugInfo = pDxilDebugInfo;
  m_pModule = pModule;

  llvm37::Function* DbgDeclareFn = llvm37::Intrinsic::getDeclaration(
      m_pModule,
      llvm37::Intrinsic::dbg_declare);

  for (llvm37::User* U : DbgDeclareFn->users())
  {
    if (auto* DbgDeclare = llvm37::dyn_cast<llvm37::DbgDeclareInst>(U))
    {
      Init_DbgDeclare(DbgDeclare);
    }
  }
}

void dxil_debug_info::LiveVariables::Impl::Init_DbgDeclare(
    llvm37::DbgDeclareInst *DbgDeclare
)
{
  llvm37::Value *Address = DbgDeclare->getAddress();
  auto *Variable = DbgDeclare->getVariable();
  auto *Expression = DbgDeclare->getExpression();

  if (Address == nullptr || Variable == nullptr || Expression == nullptr)
  {
    return;
  }

  auto* AddressAsAlloca = llvm37::dyn_cast<llvm37::AllocaInst>(Address);
  if (AddressAsAlloca == nullptr)
  {
      return;
  }

  auto *S = Variable->getScope();
  if (S == nullptr)
  {
    return;
  }

  auto Iter = CreateMemberIterator(
      DbgDeclare,
      m_pModule->getDataLayout(),
      AddressAsAlloca,
      Expression);

  if (!Iter)
  {
    // MemberIterator creation failure, this skip this var.
    return;
  }


  unsigned FragmentIndex;
  while (Iter->Next(&FragmentIndex))
  {
    const unsigned FragmentSizeInBits = 
      Iter->SizeInBits(FragmentIndex);
    const unsigned FragmentOffsetInBits =
        Iter->OffsetInBits(FragmentIndex);

    VariableInfo* VarInfo = AssignValueToOffset(
        &m_LiveVarsDbgDeclare[S],
        Variable,
        Address,
        FragmentIndex,
        FragmentOffsetInBits);

    // SROA can split structs so that multiple allocas back the same variable.
    // In this case the expression will be empty
    if (Expression->getNumElements() != 0)
    {
      ValidateDbgDeclare(
        VarInfo,
        FragmentSizeInBits,
        FragmentOffsetInBits);
    }
  }
}

dxil_debug_info::VariableInfo* dxil_debug_info::LiveVariables::Impl::AssignValueToOffset(
    VariableInfoMap *VarInfoMap,
    llvm37::DIVariable *Variable,
    llvm37::Value *Address,
    unsigned FragmentIndex,
    unsigned FragmentOffsetInBits
)
{
  // FragmentIndex is the index within the alloca'd value
  // FragmentOffsetInBits is the offset within the HLSL variable
  // that maps to Address[FragmentIndex]
  auto it = VarInfoMap->find(Variable);
  if (it == VarInfoMap->end())
  {
    auto InsertIt = VarInfoMap->emplace(
        Variable,
        std::make_unique<VariableInfo>(Variable));
    assert(InsertIt.second);
    it = InsertIt.first;
  }
  auto *VarInfo = it->second.get();
  auto &FragmentLocation =
      VarInfo->m_ValueLocationMap[FragmentOffsetInBits];
  FragmentLocation.m_V = Address;
  FragmentLocation.m_FragmentIndex = FragmentIndex;

  return VarInfo;
}

dxil_debug_info::LiveVariables::LiveVariables() = default;

dxil_debug_info::LiveVariables::~LiveVariables() = default;

HRESULT dxil_debug_info::LiveVariables::Init(DxcPixDxilDebugInfo *pDxilDebugInfo) {
  Clear();
  m_pImpl->Init(pDxilDebugInfo->GetMallocNoRef(), pDxilDebugInfo, pDxilDebugInfo->GetModuleRef());
  return S_OK;
}

void dxil_debug_info::LiveVariables::Clear() {
  m_pImpl.reset(new dxil_debug_info::LiveVariables::Impl());
}

HRESULT dxil_debug_info::LiveVariables::GetLiveVariablesAtInstruction(
  llvm37::Instruction *IP,
  IDxcPixDxilLiveVariables **ppResult) const {
  DXASSERT(IP != nullptr, "else IP should not be nullptr");
  DXASSERT(ppResult != nullptr, "else Result should not be nullptr");

  std::vector<const VariableInfo *> LiveVars;
  std::set<std::string> LiveVarsName;

  const llvm37::DebugLoc &DL = IP->getDebugLoc();

  if (!DL)
  {
    return E_FAIL;
  }

  llvm37::DIScope *S = DL->getScope();
  if (S == nullptr)
  {
    return E_FAIL;
  }

  const llvm37::DITypeIdentifierMap EmptyMap;
  while (S != nullptr)
  {
    auto it = m_pImpl->m_LiveVarsDbgDeclare.find(S);
    if (it != m_pImpl->m_LiveVarsDbgDeclare.end())
    {
      for (const auto &VarAndInfo :  it->second)
      {
        auto *Var = VarAndInfo.first;
        llvm37::StringRef VarName = Var->getName();
        if (Var->getLine() > DL.getLine())
        {
          // Defined later in the HLSL source.
          continue;
        }
        if (VarName.empty())
        {
          // No name?...
          continue;
        }
        if (!LiveVarsName.insert(VarName).second)
        {
          // There's a variable with the same name; use the
          // previous one instead.
          continue;
        }

        LiveVars.emplace_back(VarAndInfo.second.get());
      }
    }
    S = S->getScope().resolve(EmptyMap);
  }

  return CreateDxilLiveVariables(
      m_pImpl->m_pDxilDebugInfo,
      std::move(LiveVars),
      ppResult);
}
