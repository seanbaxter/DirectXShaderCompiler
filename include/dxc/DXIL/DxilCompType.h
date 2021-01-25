///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilCompType.h                                                            //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Represenation of HLSL component type.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "DxilConstants.h"

namespace llvm37 {
class Type;
class PointerType;
class LLVMContext;
}


namespace hlsl {

/// Use this class to represent HLSL component/element types.
class CompType {
public:
  using Kind = DXIL::ComponentType;

  CompType();
  CompType(Kind K);
  CompType(unsigned int K);

  bool operator==(const CompType &o) const;

  Kind GetKind() const;
  uint8_t GetSizeInBits() const;

  static CompType getInvalid();
  static CompType getF16();
  static CompType getF32();
  static CompType getF64();
  static CompType getI16();
  static CompType getI32();
  static CompType getI64();
  static CompType getU16();
  static CompType getU32();
  static CompType getU64();
  static CompType getI1();
  static CompType getSNormF16();
  static CompType getUNormF16();
  static CompType getSNormF32();
  static CompType getUNormF32();
  static CompType getSNormF64();
  static CompType getUNormF64();

  bool IsInvalid() const;
  bool IsFloatTy() const;
  bool IsIntTy() const;
  bool IsSIntTy() const;
  bool IsUIntTy() const;
  bool IsBoolTy() const;

  bool IsSNorm() const;
  bool IsUNorm() const;
  bool Is64Bit() const;
  bool Is16Bit() const;

  /// For min-precision types, returns upconverted (base) type.
  CompType GetBaseCompType() const;
  bool HasMinPrec() const;
  llvm37::Type *GetLLVMType(llvm37::LLVMContext &Ctx) const;
  llvm37::PointerType *GetLLVMPtrType(llvm37::LLVMContext &Ctx, const unsigned AddrSpace = 0) const;
  llvm37::Type *GetLLVMBaseType(llvm37::LLVMContext &Ctx) const;

  /// Get the component type for a given llvm type.
  ///
  /// LLVM types do not hold sign information so there is no 1-1
  /// correspondence between llvm types and component types. 
  /// This method returns the signed version for all integer
  /// types.
  /// 
  /// TODO: decide if we should distinguish between signed
  ///       and unsigned types in this api.
  static CompType GetCompType(llvm37::Type * type);

  const char *GetName() const;
  const char *GetHLSLName(bool MinPrecision) const;

private:
  Kind m_Kind;
};

} // namespace hlsl
