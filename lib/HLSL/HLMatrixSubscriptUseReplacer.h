///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLMatrixSubscriptUseReplacer.h                                            //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "llvm37/ADT/SmallVector.h"
#include "llvm37/IR/IRBuilder.h"
#include <vector>

namespace llvm37 {
class Value;
class AllocaInst;
class CallInst;
class Instruction;
class Function;
} // namespace llvm37

namespace hlsl {
// Implements recursive replacement of a matrix subscript's uses,
// from a pointer to a matrix value to a pointer to its lowered vector version,
// whether directly or through GEPs in the case of two-level indexing like mat[i][j].
// This has to handle one or two levels of indices, each of which either
// constant or dynamic: mat[0], mat[i], mat[0][0], mat[i][0], mat[0][j], mat[i][j],
// plus the equivalent element accesses: mat._11, mat._11_12, mat._11_12[0], mat._11_12[i]
class HLMatrixSubscriptUseReplacer {
public:
  // The constructor does everything
  HLMatrixSubscriptUseReplacer(llvm37::CallInst* Call, llvm37::Value *LoweredPtr, llvm37::Value *TempLoweredMatrix,
    llvm37::SmallVectorImpl<llvm37::Value*> &ElemIndices, bool AllowLoweredPtrGEPs,
    std::vector<llvm37::Instruction*> &DeadInsts);

private:
  void replaceUses(llvm37::Instruction* PtrInst, llvm37::Value* SubIdxVal);
  llvm37::Value *tryGetScalarIndex(llvm37::Value *SubIdxVal, llvm37::IRBuilder<> &Builder);
  void cacheLoweredMatrix(bool ForDynamicIndexing, llvm37::IRBuilder<> &Builder);
  llvm37::Value *loadElem(llvm37::Value *Idx, llvm37::IRBuilder<> &Builder);
  void storeElem(llvm37::Value *Idx, llvm37::Value *Elem, llvm37::IRBuilder<> &Builder);
  llvm37::Value *loadVector(llvm37::IRBuilder<> &Builder);
  void storeVector(llvm37::Value *Vec, llvm37::IRBuilder<> &Builder);
  void flushLoweredMatrix(llvm37::IRBuilder<> &Builder);

private:
  llvm37::Value *LoweredPtr;
  llvm37::SmallVectorImpl<llvm37::Value*> &ElemIndices;
  std::vector<llvm37::Instruction*> &DeadInsts;
  bool AllowLoweredPtrGEPs = false;
  bool HasScalarResult = false;
  bool HasDynamicElemIndex = false;
  llvm37::Type *LoweredTy = nullptr;

  // The entire lowered matrix as loaded from LoweredPtr,
  // nullptr if we copied it to a temporary array.
  llvm37::Value *TempLoweredMatrix = nullptr;

  // We allocate this if the level 1 indices are not all constants,
  // so we can dynamically index the lowered matrix vector.
  llvm37::AllocaInst *LazyTempElemArrayAlloca = nullptr;

  // We'll allocate this lazily if we have a dynamic level 2 index (mat[0][i]),
  // so we can dynamically index the level 1 indices.
  llvm37::AllocaInst *LazyTempElemIndicesArrayAlloca = nullptr;
};
} // namespace hlsl
