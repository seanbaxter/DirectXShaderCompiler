///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLMatrixType.h                                                            //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "llvm37/IR/IRBuilder.h"

namespace llvm37 {
  template<typename T>
  class ArrayRef;
  class Type;
  class Value;
  class Constant;
  class StructType;
  class VectorType;
  class StoreInst;
}

namespace hlsl {

class DxilFieldAnnotation;
class DxilTypeSystem;

// A high-level matrix type in LLVM IR.
//
// Matrices are represented by an llvm struct type of the following form:
// { [RowCount x <ColCount x RegReprTy>] }
// Note that the element type is always in its register representation (ie bools are i1s).
// This allows preserving the original type and is okay since matrix types are only
// manipulated in an opaque way, through intrinsics.
//
// During matrix lowering, matrices are converted to vectors of the following form:
// <RowCount*ColCount x Ty>
// At this point, register vs memory representation starts to matter and we have to
// imitate the codegen for scalar and vector bools: i1s when in llvm registers,
// and i32s when in memory (allocas, pointers, or in structs/lists, which are always in memory).
//
// This class is designed to resemble a llvm37::Type-derived class.
class HLMatrixType
{
public:
  static constexpr const char* StructNamePrefix = "class.matrix.";

  HLMatrixType() : RegReprElemTy(nullptr), NumRows(0), NumColumns(0) {}
  HLMatrixType(llvm37::Type *RegReprElemTy, unsigned NumRows, unsigned NumColumns);

  // We allow default construction to an invalid state to support the dynCast pattern.
  // This tests whether we have a legit object.
  operator bool() const { return RegReprElemTy != nullptr; }

  llvm37::Type *getElementType(bool MemRepr) const;
  llvm37::Type *getElementTypeForReg() const { return getElementType(false); }
  llvm37::Type *getElementTypeForMem() const { return getElementType(true); }
  unsigned getNumRows() const { return NumRows; }
  unsigned getNumColumns() const { return NumColumns; }
  unsigned getNumElements() const { return NumRows * NumColumns; }
  unsigned getRowMajorIndex(unsigned RowIdx, unsigned ColIdx) const;
  unsigned getColumnMajorIndex(unsigned RowIdx, unsigned ColIdx) const;
  static unsigned getRowMajorIndex(unsigned RowIdx, unsigned ColIdx, unsigned NumRows, unsigned NumColumns);
  static unsigned getColumnMajorIndex(unsigned RowIdx, unsigned ColIdx, unsigned NumRows, unsigned NumColumns);

  llvm37::VectorType *getLoweredVectorType(bool MemRepr) const;
  llvm37::VectorType *getLoweredVectorTypeForReg() const { return getLoweredVectorType(false); }
  llvm37::VectorType *getLoweredVectorTypeForMem() const { return getLoweredVectorType(true); }

  llvm37::Value *emitLoweredMemToReg(llvm37::Value *Val, llvm37::IRBuilder<> &Builder) const;
  llvm37::Value *emitLoweredRegToMem(llvm37::Value *Val, llvm37::IRBuilder<> &Builder) const;
  llvm37::Value *emitLoweredLoad(llvm37::Value *Ptr, llvm37::IRBuilder<> &Builder) const;
  llvm37::StoreInst *emitLoweredStore(llvm37::Value *Val, llvm37::Value *Ptr, llvm37::IRBuilder<> &Builder) const;

  llvm37::Value *emitLoweredVectorRowToCol(llvm37::Value *VecVal, llvm37::IRBuilder<> &Builder) const;
  llvm37::Value *emitLoweredVectorColToRow(llvm37::Value *VecVal, llvm37::IRBuilder<> &Builder) const;

  static bool isa(llvm37::Type *Ty);
  static bool isMatrixPtr(llvm37::Type *Ty);
  static bool isMatrixArray(llvm37::Type *Ty);
  static bool isMatrixArrayPtr(llvm37::Type *Ty);
  static bool isMatrixPtrOrArrayPtr(llvm37::Type *Ty);
  static bool isMatrixOrPtrOrArrayPtr(llvm37::Type *Ty);

  static llvm37::Type *getLoweredType(llvm37::Type *Ty, bool MemRepr = false);

  static HLMatrixType cast(llvm37::Type *Ty);
  static HLMatrixType dyn_cast(llvm37::Type *Ty);

private:
  llvm37::Type *RegReprElemTy;
  unsigned NumRows, NumColumns;
};

} // namespace hlsl