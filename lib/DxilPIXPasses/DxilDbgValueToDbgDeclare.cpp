///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilDbgValueToDbgDeclare.cpp                                              //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Converts calls to llvm37.dbg.value to llvm37.dbg.declare + alloca + stores.   //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <algorithm>
#include <memory>
#include <map>
#include <unordered_map>
#include <utility>

#include "dxc/DXIL/DxilConstants.h"
#include "dxc/DXIL/DxilResourceBase.h"
#include "dxc/DXIL/DxilModule.h"
#include "dxc/DxilPIXPasses/DxilPIXPasses.h"
#include "llvm37/IR/DebugInfo.h"
#include "llvm37/IR/DebugInfoMetadata.h"
#include "llvm37/IR/Instructions.h"
#include "llvm37/IR/IntrinsicInst.h"
#include "llvm37/IR/Intrinsics.h"
#include "llvm37/IR/IRBuilder.h"
#include "llvm37/IR/Module.h"
#include "llvm37/Pass.h"

#define DEBUG_TYPE "dxil-dbg-value-to-dbg-declare"

namespace {
using OffsetInBits = unsigned;
using SizeInBits = unsigned;

// OffsetManager is used to map between "packed" and aligned offsets.
//
// For example, the aligned offsets for a struct [float, half, int, double]
// will be {0, 32, 64, 128} (assuming 32 bit alignments for ints, and 64
// bit for doubles), while the packed offsets will be {0, 32, 48, 80}.
//
// This mapping makes it easier to deal with llvm37.dbg.values whose value
// operand does not match exactly the Variable operand's type.
class OffsetManager
{
public:
  OffsetManager() = default;

  // AlignTo aligns the current aligned offset to Ty's natural alignment.
  void AlignTo(
      llvm37::DIType *Ty
  )
  {
    // This is some magic arithmetic. Here's an example:
    //
    // Assume the natural alignment for Ty is 16 bits. Then
    //
    //     AlignMask = 0x0000000f(15)
    //
    // If the current aligned offset is 
    //
    //     CurrentAlignedOffset = 0x00000048(72)
    //
    // Then
    //
    //     T = CurrentAlignOffset + AlignMask = 0x00000057(87)
    //
    // Which mean
    //
    //     T & ~CurrentOffset = 0x00000050(80)
    //
    // is the aligned offset where Ty should be placed.
    unsigned AlignMask = Ty->getAlignInBits();

    if (AlignMask == 0)
    {
      if (auto *DerivedTy = llvm37::dyn_cast<llvm37::DIDerivedType>(Ty)) {
        const llvm37::DITypeIdentifierMap EmptyMap;
        switch (DerivedTy->getTag()) {
        case llvm37::dwarf::DW_TAG_restrict_type:
        case llvm37::dwarf::DW_TAG_reference_type:
        case llvm37::dwarf::DW_TAG_const_type:
        case llvm37::dwarf::DW_TAG_typedef:
            AlignMask = DerivedTy->getBaseType().resolve(EmptyMap)->getAlignInBits();
            assert(AlignMask != 0);
        }
      }
    }
    AlignMask = AlignMask - 1;
    m_CurrentAlignedOffset =
        (m_CurrentAlignedOffset + AlignMask) & ~AlignMask;
  }

  // Add is used to "add" an aggregate element (struct field, array element)
  // at the current aligned/packed offsets, bumping them by Ty's size.
  OffsetInBits Add(
      llvm37::DIBasicType *Ty
  )
  {
    m_PackedOffsetToAlignedOffset[m_CurrentPackedOffset] = m_CurrentAlignedOffset;
    m_AlignedOffsetToPackedOffset[m_CurrentAlignedOffset] = m_CurrentPackedOffset;

    const OffsetInBits Ret = m_CurrentAlignedOffset;
    m_CurrentPackedOffset += Ty->getSizeInBits();
    m_CurrentAlignedOffset += Ty->getSizeInBits();

    return Ret;
  }

  // AlignToAndAddUnhandledType is used for error handling when Ty
  // could not be handled by the transformation. This is a best-effort
  // way to continue the pass by ignoring the current type and hoping
  // that adding Ty as a blob other fields/elements added will land
  // in the proper offset.
  void AlignToAndAddUnhandledType(
      llvm37::DIType *Ty
  )
  {
    AlignTo(Ty);
    m_CurrentPackedOffset += Ty->getSizeInBits();
    m_CurrentAlignedOffset += Ty->getSizeInBits();
  }

  void AddResourceType(llvm37::DIType *Ty)
  {
    m_PackedOffsetToAlignedOffset[m_CurrentPackedOffset] =
        m_CurrentAlignedOffset;
    m_AlignedOffsetToPackedOffset[m_CurrentAlignedOffset] =
        m_CurrentPackedOffset;

    m_CurrentPackedOffset += Ty->getSizeInBits();
    m_CurrentAlignedOffset += Ty->getSizeInBits();
  }

  bool GetAlignedOffsetFromPackedOffset(
      OffsetInBits PackedOffset,
      OffsetInBits *AlignedOffset
  ) const
  {
    return GetOffsetWithMap(
        m_PackedOffsetToAlignedOffset,
        PackedOffset,
        AlignedOffset);
  }

  OffsetInBits GetPackedOffsetFromAlignedOffset(
      OffsetInBits AlignedOffset,
      OffsetInBits *PackedOffset
  ) const
  {
    return GetOffsetWithMap(
        m_PackedOffsetToAlignedOffset,
        AlignedOffset,
        PackedOffset);
  }

  OffsetInBits GetCurrentPackedOffset() const
  {
    return m_CurrentPackedOffset;
  }

  OffsetInBits GetCurrentAlignedOffset() const
  {
      return m_CurrentAlignedOffset;
  }

private:
  OffsetInBits m_CurrentPackedOffset = 0;
  OffsetInBits m_CurrentAlignedOffset = 0;

  using OffsetMap = std::unordered_map<OffsetInBits, OffsetInBits>;

  OffsetMap m_PackedOffsetToAlignedOffset;
  OffsetMap m_AlignedOffsetToPackedOffset;

  static bool GetOffsetWithMap(
      const OffsetMap &Map,
      OffsetInBits SrcOffset,
      OffsetInBits *DstOffset
  )
  {
    auto it = Map.find(SrcOffset);
    if (it == Map.end())
    {
      return false;
    }

    *DstOffset = it->second;
    return true;
  }
};

// VariableRegisters contains the logic for traversing a DIType T and
// creating AllocaInsts that map back to a specific offset within T.
class VariableRegisters
{
public:
  VariableRegisters(
      llvm37::DebugLoc const &,
      llvm37::DIVariable *Variable,
      llvm37::Module *M
  );

  llvm37::AllocaInst *GetRegisterForAlignedOffset(
      OffsetInBits AlignedOffset
  ) const;

  const OffsetManager& GetOffsetManager() const
  {
    return m_Offsets;
  }

private:
  void PopulateAllocaMap(
      llvm37::DIType *Ty
  );

  void PopulateAllocaMap_BasicType(llvm37::DIBasicType *Ty
  );

  void PopulateAllocaMap_ArrayType(llvm37::DICompositeType *Ty
  );

  void PopulateAllocaMap_StructType(llvm37::DICompositeType *Ty
  );

  llvm37::DILocation *GetVariableLocation() const;
  llvm37::Value *GetMetadataAsValue(
      llvm37::Metadata *M
  ) const;
  llvm37::DIExpression *GetDIExpression(
      llvm37::DIType *Ty,
      OffsetInBits Offset
  ) const;

  llvm37::DebugLoc const &m_dbgLoc;
  llvm37::DIVariable *m_Variable = nullptr;
  llvm37::IRBuilder<> m_B;
  llvm37::Function *m_DbgDeclareFn = nullptr;

  OffsetManager m_Offsets;
  std::unordered_map<OffsetInBits, llvm37::AllocaInst *> m_AlignedOffsetToAlloca;
};

class DxilDbgValueToDbgDeclare : public llvm37::ModulePass {
public:
    static char ID;
    DxilDbgValueToDbgDeclare() : llvm37::ModulePass(ID)
    {
    }
  bool runOnModule(
      llvm37::Module &M
  ) override;

private:
  void handleDbgValue(
      llvm37::Module &M,
      llvm37::DbgValueInst *DbgValue);

  std::unordered_map<llvm37::DIVariable *, std::unique_ptr<VariableRegisters>> m_Registers;
};
}  // namespace

char DxilDbgValueToDbgDeclare::ID = 0;

struct ValueAndOffset
{
    llvm37::Value *m_V;
    OffsetInBits m_PackedOffset;
};

// SplitValue splits an llvm37::Value into possibly multiple
// scalar Values. Those scalar values will later be "stored"
// into their corresponding register.
static OffsetInBits SplitValue(
    llvm37::Value *V,
    OffsetInBits CurrentOffset,
    std::vector<ValueAndOffset> *Values,
    llvm37::IRBuilder<>& B
)
{
  auto *VTy = V->getType();
  if (auto *ArrTy = llvm37::dyn_cast<llvm37::ArrayType>(VTy))
  {
    for (unsigned i = 0; i < ArrTy->getNumElements(); ++i)
    {
      CurrentOffset = SplitValue(
          B.CreateExtractValue(V, {i}),
          CurrentOffset,
          Values,
          B);
    }
  }
  else if (auto *StTy = llvm37::dyn_cast<llvm37::StructType>(VTy))
  {
    for (unsigned i = 0; i < StTy->getNumElements(); ++i)
    {
      CurrentOffset = SplitValue(
          B.CreateExtractValue(V, {i}),
          CurrentOffset,
          Values,
          B);
    }
  }
  else if (auto *VecTy = llvm37::dyn_cast<llvm37::VectorType>(VTy))
  {
    for (unsigned i = 0; i < VecTy->getNumElements(); ++i)
    {
      CurrentOffset = SplitValue(
          B.CreateExtractElement(V, i),
          CurrentOffset,
          Values,
          B);
    }
  }
  else
  {
    assert(VTy->isFloatTy() || VTy->isDoubleTy() || VTy->isHalfTy() ||
           VTy->isIntegerTy(32) || VTy->isIntegerTy(64) || VTy->isIntegerTy(16));
    Values->emplace_back(ValueAndOffset{V, CurrentOffset});
    CurrentOffset += VTy->getScalarSizeInBits();
  }

  return CurrentOffset;
}

// A more convenient version of SplitValue.
static std::vector<ValueAndOffset> SplitValue(
    llvm37::Value* V,
    OffsetInBits CurrentOffset,
    llvm37::IRBuilder<>& B
)
{
    std::vector<ValueAndOffset> Ret;
    SplitValue(V, CurrentOffset, &Ret, B);
    return Ret;
}

// Convenient helper for parsing a DIExpression's offset.
static OffsetInBits GetAlignedOffsetFromDIExpression(
    llvm37::DIExpression *Exp
)
{
  if (!Exp->isBitPiece())
  {
    return 0;
  }

  return Exp->getBitPieceOffset();
}

bool DxilDbgValueToDbgDeclare::runOnModule(
    llvm37::Module &M
)
{
  auto *DbgValueFn =
      llvm37::Intrinsic::getDeclaration(&M, llvm37::Intrinsic::dbg_value);

  bool Changed = false;
  for (auto it = DbgValueFn->user_begin(); it != DbgValueFn->user_end(); )
  {
    llvm37::User *User = *it++;

    if (auto *DbgValue = llvm37::dyn_cast<llvm37::DbgValueInst>(User))
    {
      Changed = true;
      handleDbgValue(M, DbgValue);
      DbgValue->eraseFromParent();
    }
  }
  return Changed;
}

void DxilDbgValueToDbgDeclare::handleDbgValue(
    llvm37::Module& M,
    llvm37::DbgValueInst* DbgValue)
{
  llvm37::Value *V = DbgValue->getValue();
  if (V == nullptr) {
    // The metadata contained a null Value, so we ignore it. This
    // seems to be a dxcompiler bug.
    return;
  }

  if (auto *PtrTy = llvm37::dyn_cast<llvm37::PointerType>(V->getType())) {
    return;
  }
  
  llvm37::DIVariable *Variable = DbgValue->getVariable();
  auto &Register = m_Registers[Variable];
  if (Register == nullptr)
  {
    Register.reset(new VariableRegisters(DbgValue->getDebugLoc(), Variable, &M));
  }

  // Convert the offset from DbgValue's expression to a packed
  // offset, which we'll need in order to determine the (packed)
  // offset of each scalar Value in DbgValue.
  llvm37::DIExpression* expression = DbgValue->getExpression();
  const OffsetInBits AlignedOffsetFromVar =
      GetAlignedOffsetFromDIExpression(expression);
  OffsetInBits PackedOffsetFromVar;
  const OffsetManager& Offsets = Register->GetOffsetManager();
  if (!Offsets.GetPackedOffsetFromAlignedOffset(AlignedOffsetFromVar,
                                                &PackedOffsetFromVar))
  {
    // todo: output geometry for GS
    return;
  }

  const OffsetInBits InitialOffset = PackedOffsetFromVar;
  llvm37::IRBuilder<> B(DbgValue->getCalledFunction()->getContext());
  B.SetInsertPoint(DbgValue);
  B.SetCurrentDebugLocation(llvm37::DebugLoc());
  auto *Zero = B.getInt32(0);

  // Now traverse a list of pairs {Scalar Value, InitialOffset + Offset}.
  // InitialOffset is the offset from DbgValue's expression (i.e., the
  // offset from the Variable's start), and Offset is the Scalar Value's
  // packed offset from DbgValue's value. 
  for (const ValueAndOffset &VO : SplitValue(V, InitialOffset, B))
  {
    OffsetInBits AlignedOffset;
    if (!Offsets.GetAlignedOffsetFromPackedOffset(VO.m_PackedOffset,
                                                  &AlignedOffset))
    {
      continue;
    }

    auto* AllocaInst = Register->GetRegisterForAlignedOffset(AlignedOffset);
    if (AllocaInst == nullptr)
    {
      assert(!"Failed to find alloca for var[offset]");
      continue;
    }

    if (AllocaInst->getAllocatedType()->getArrayElementType() == VO.m_V->getType())
    {
      auto* GEP = B.CreateGEP(AllocaInst, { Zero, Zero });
      B.CreateStore(VO.m_V, GEP);
    }
  }
}


llvm37::AllocaInst *VariableRegisters::GetRegisterForAlignedOffset(
    OffsetInBits Offset
) const
{
  auto it = m_AlignedOffsetToAlloca.find(Offset);
  if (it == m_AlignedOffsetToAlloca.end())
  {
    return nullptr;
  }
  return it->second;
}

#ifndef NDEBUG
// DITypePeelTypeAlias peels const, typedef, and other alias types off of Ty,
// returning the unalised type.
static llvm37::DIType *DITypePeelTypeAlias(
    llvm37::DIType* Ty
)
{
  if (auto *DerivedTy = llvm37::dyn_cast<llvm37::DIDerivedType>(Ty))
  {
    const llvm37::DITypeIdentifierMap EmptyMap;
    switch (DerivedTy->getTag())
    {
    case llvm37::dwarf::DW_TAG_restrict_type:
    case llvm37::dwarf::DW_TAG_reference_type:
    case llvm37::dwarf::DW_TAG_const_type:
    case llvm37::dwarf::DW_TAG_typedef:
    case llvm37::dwarf::DW_TAG_pointer_type:
    case llvm37::dwarf::DW_TAG_member:
      return DITypePeelTypeAlias(
          DerivedTy->getBaseType().resolve(EmptyMap));
    }
  }

  return Ty;
}
#endif // NDEBUG

VariableRegisters::VariableRegisters(
    llvm37::DebugLoc const & dbgLoc,
    llvm37::DIVariable *Variable,
    llvm37::Module *M)
  : m_dbgLoc(dbgLoc)
  ,m_Variable(Variable)
  , m_B(M->GetOrCreateDxilModule().GetEntryFunction()->getEntryBlock().begin())
  , m_DbgDeclareFn(llvm37::Intrinsic::getDeclaration(
      M, llvm37::Intrinsic::dbg_declare))
{
  const llvm37::DITypeIdentifierMap EmptyMap;
  llvm37::DIType* Ty = m_Variable->getType().resolve(EmptyMap);

  PopulateAllocaMap(Ty);
  assert(m_Offsets.GetCurrentPackedOffset() ==
         DITypePeelTypeAlias(Ty)->getSizeInBits());
}

void VariableRegisters::PopulateAllocaMap(
    llvm37::DIType *Ty
)
{
  if (auto *DerivedTy = llvm37::dyn_cast<llvm37::DIDerivedType>(Ty))
  {
    const llvm37::DITypeIdentifierMap EmptyMap;
    switch (DerivedTy->getTag())
    {
    default:
      assert(!"Unhandled DIDerivedType");
      m_Offsets.AlignToAndAddUnhandledType(DerivedTy);
      return;
    case llvm37::dwarf::DW_TAG_arg_variable: // "this" pointer
    case llvm37::dwarf::DW_TAG_pointer_type: // "this" pointer
    case llvm37::dwarf::DW_TAG_restrict_type:
    case llvm37::dwarf::DW_TAG_reference_type:
    case llvm37::dwarf::DW_TAG_const_type:
    case llvm37::dwarf::DW_TAG_typedef:
    case llvm37::dwarf::DW_TAG_member:
      PopulateAllocaMap(
          DerivedTy->getBaseType().resolve(EmptyMap));
      return;
    case llvm37::dwarf::DW_TAG_subroutine_type:
        //ignore member functions.
      return;
    }
  }
  else if (auto *CompositeTy = llvm37::dyn_cast<llvm37::DICompositeType>(Ty))
  {
    switch (CompositeTy->getTag())
    {
    default:
      assert(!"Unhandled DICompositeType");
      m_Offsets.AlignToAndAddUnhandledType(CompositeTy);
      return;
    case llvm37::dwarf::DW_TAG_array_type:
      PopulateAllocaMap_ArrayType(CompositeTy);
      return;
    case llvm37::dwarf::DW_TAG_structure_type:
    case llvm37::dwarf::DW_TAG_class_type:
      PopulateAllocaMap_StructType(CompositeTy);
      return;
    }
  }
  else if (auto *BasicTy = llvm37::dyn_cast<llvm37::DIBasicType>(Ty))
  {
    PopulateAllocaMap_BasicType(BasicTy);
    return;
  }

  assert(!"Unhandled DIType");
  m_Offsets.AlignToAndAddUnhandledType(Ty);
}

static llvm37::Type* GetLLVM37TypeFromDIBasicType(
    llvm37::IRBuilder<> &B,
    llvm37::DIBasicType* Ty
)
{
  const SizeInBits Size = Ty->getSizeInBits();

  switch (Ty->getEncoding())
  {
  default:
    break;

  case llvm37::dwarf::DW_ATE_boolean:
  case llvm37::dwarf::DW_ATE_signed:
  case llvm37::dwarf::DW_ATE_unsigned:
    switch(Size)
    {
    case 16:
      return B.getInt16Ty();
    case 32:
      return B.getInt32Ty();
    case 64:
      return B.getInt64Ty();
    }
    break;
  case llvm37::dwarf::DW_ATE_float:
    switch(Size)
    {
    case 16:
      return B.getHalfTy();
    case 32:
      return B.getFloatTy();
    case 64:
      return B.getDoubleTy();
    }
    break;
  }

  return nullptr;
}

void VariableRegisters::PopulateAllocaMap_BasicType(
    llvm37::DIBasicType *Ty
)
{
  llvm37::Type* AllocaElementTy = GetLLVM37TypeFromDIBasicType(m_B, Ty);
  assert(AllocaElementTy != nullptr);
  if (AllocaElementTy == nullptr)
  {
      return;
  }

  const OffsetInBits AlignedOffset = m_Offsets.Add(Ty);

  llvm37::Type *AllocaTy = llvm37::ArrayType::get(AllocaElementTy, 1);
  llvm37::AllocaInst *&Alloca = m_AlignedOffsetToAlloca[AlignedOffset];
  Alloca = m_B.CreateAlloca(AllocaTy, m_B.getInt32(0));
  Alloca->setDebugLoc(llvm37::DebugLoc());

  auto *Storage = GetMetadataAsValue(llvm37::ValueAsMetadata::get(Alloca));
  auto *Variable = GetMetadataAsValue(m_Variable);
  auto *Expression = GetMetadataAsValue(GetDIExpression(Ty, AlignedOffset));
  auto *DbgDeclare = m_B.CreateCall(
      m_DbgDeclareFn,
      {Storage, Variable, Expression});
  DbgDeclare->setDebugLoc(m_dbgLoc);
}

static unsigned NumArrayElements(
    llvm37::DICompositeType *Array
)
{
  if (Array->getElements().size() == 0)
  {
    return 0;
  }

  unsigned NumElements = 1;
  for (llvm37::DINode *N : Array->getElements())
  {
    if (auto* Subrange = llvm37::dyn_cast<llvm37::DISubrange>(N))
    {
      NumElements *= Subrange->getCount();
    }
    else
    {
      assert(!"Unhandled array element");
      return 0;
    }
  }
  return NumElements;
}

void VariableRegisters::PopulateAllocaMap_ArrayType(
    llvm37::DICompositeType* Ty
)
{
  unsigned NumElements = NumArrayElements(Ty);
  if (NumElements == 0)
  {
    m_Offsets.AlignToAndAddUnhandledType(Ty);
    return;
  }

  const SizeInBits ArraySizeInBits = Ty->getSizeInBits();
  (void)ArraySizeInBits;

  const llvm37::DITypeIdentifierMap EmptyMap;
  llvm37::DIType *ElementTy = Ty->getBaseType().resolve(EmptyMap);
  assert(ArraySizeInBits % NumElements == 0 &&
         " invalid DIArrayType"
         " - Size is not a multiple of NumElements");

  // After aligning the current aligned offset to ElementTy's natural
  // alignment, the current aligned offset must match Ty's offset
  // in bits.
  m_Offsets.AlignTo(ElementTy);

  for (unsigned i = 0; i < NumElements; ++i)
  {
    // This is only needed if ElementTy's size is not a multiple of
    // its natural alignment.
    m_Offsets.AlignTo(ElementTy);
    PopulateAllocaMap(ElementTy);
  }
}

// SortMembers traverses all of Ty's members and returns them sorted
// by their offset from Ty's start. Returns true if the function succeeds
// and false otherwise.
static bool SortMembers(
    llvm37::DICompositeType* Ty,
    std::map<OffsetInBits, llvm37::DIDerivedType*> *SortedMembers
)
{
    auto Elements = Ty->getElements();
    if (Elements.begin() == Elements.end())
    {
        return false;
    }
    for (auto* Element : Elements)
    {
        switch (Element->getTag())
        {
        case llvm37::dwarf::DW_TAG_member: {
            if (auto* Member = llvm37::dyn_cast<llvm37::DIDerivedType>(Element))
            {
                if (Member->getSizeInBits()) {
                    auto it = SortedMembers->emplace(std::make_pair(Member->getOffsetInBits(), Member));
                    (void)it;
                    assert(it.second &&
                        "Invalid DIStructType"
                        " - members with the same offset -- are unions possible?");
                }
                break;
            }
            assert(!"member is not a Member");
            return false;
        }
        case llvm37::dwarf::DW_TAG_subprogram: {
            if (auto* SubProgram = llvm37::dyn_cast<llvm37::DISubprogram>(Element)) {
                continue;
            }
            assert(!"DISubprogram not understood");
            return false;
        }
        case llvm37::dwarf::DW_TAG_inheritance: {
            if (auto* Member = llvm37::dyn_cast<llvm37::DIDerivedType>(Element))
            {
                auto it = SortedMembers->emplace(
                    std::make_pair(Member->getOffsetInBits(), Member));
                (void)it;
                assert(it.second &&
                    "Invalid DIStructType"
                    " - members with the same offset -- are unions possible?");
            }
            continue;
        }
        default:
            assert(!"Unhandled field type in DIStructType");
            return false;
        }
    }
  return true;
}

static bool IsResourceObject(llvm37::DIDerivedType *DT) {
  const llvm37::DITypeIdentifierMap EmptyMap;
  auto *BT = DT->getBaseType().resolve(EmptyMap);
  if (auto *CompositeTy = llvm37::dyn_cast<llvm37::DICompositeType>(BT)) {
    // Resource variables (e.g. TextureCube) are composite types but have no elements:
    if (CompositeTy->getElements().begin() ==
        CompositeTy->getElements().end()) {
      auto name = CompositeTy->getName();
      auto openTemplateListMarker = name.find_first_of('<');
      if (openTemplateListMarker != llvm37::StringRef::npos) {
        auto hlslType = name.substr(0, openTemplateListMarker);
        for (int i = static_cast<int>(hlsl::DXIL::ResourceKind::Invalid) + 1;
            i < static_cast<int>(hlsl::DXIL::ResourceKind::NumEntries);
            ++i) {
          if (hlslType == hlsl::GetResourceKindName(static_cast<hlsl::DXIL::ResourceKind>(i))) {
            return true;
          }
        }
      }
    }
  }
  return false;
}

void VariableRegisters::PopulateAllocaMap_StructType(
    llvm37::DICompositeType *Ty
)
{
  std::map<OffsetInBits, llvm37::DIDerivedType*> SortedMembers;
  if (!SortMembers(Ty, &SortedMembers))
  {
      m_Offsets.AlignToAndAddUnhandledType(Ty);
      return;
  }

  m_Offsets.AlignTo(Ty);
  const OffsetInBits StructStart = m_Offsets.GetCurrentAlignedOffset();
  (void)StructStart;
  const llvm37::DITypeIdentifierMap EmptyMap;

  for (auto OffsetAndMember : SortedMembers)
  {
    // Align the offsets to the member's type natural alignment. This
    // should always result in the current aligned offset being the
    // same as the member's offset.
    m_Offsets.AlignTo(OffsetAndMember.second);
    assert(m_Offsets.GetCurrentAlignedOffset() ==
        StructStart + OffsetAndMember.first &&
        "Offset mismatch in DIStructType");
    if (IsResourceObject(OffsetAndMember.second)) {
      m_Offsets.AddResourceType(OffsetAndMember.second);
    } else {
      PopulateAllocaMap(
          OffsetAndMember.second->getBaseType().resolve(EmptyMap));
    }
  }
}

llvm37::DILocation *VariableRegisters::GetVariableLocation() const
{
  const unsigned DefaultColumn = 1;
  return llvm37::DILocation::get(
      m_B.getContext(),
      m_Variable->getLine(),
      DefaultColumn,
      m_Variable->getScope());
}

llvm37::Value *VariableRegisters::GetMetadataAsValue(
    llvm37::Metadata *M
) const
{
  return llvm37::MetadataAsValue::get(m_B.getContext(), M);
}

llvm37::DIExpression *VariableRegisters::GetDIExpression(
    llvm37::DIType *Ty,
    OffsetInBits Offset
) const
{
  llvm37::SmallVector<uint64_t, 3> ExpElements;
  if (Offset != 0)
  {
    ExpElements.emplace_back(llvm37::dwarf::DW_OP_bit_piece);
    ExpElements.emplace_back(Offset);
    ExpElements.emplace_back(Ty->getSizeInBits());
  }
  return llvm37::DIExpression::get(m_B.getContext(), ExpElements);
}

using namespace llvm37;

INITIALIZELLVM37_PASS(DxilDbgValueToDbgDeclare, DEBUG_TYPE,
                "Converts calls to dbg.value to dbg.declare + stores to new virtual registers",
                false, false)

ModulePass *llvm37::createDxilDbgValueToDbgDeclarePass() {
  return new DxilDbgValueToDbgDeclare();
}
