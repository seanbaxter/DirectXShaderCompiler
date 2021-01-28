///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLLowerUDT.h                                                              //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// Lower user defined type used directly by certain intrinsic operations.    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once
#include "dxc/Support/Global.h"
#include "dxc/DXIL/DxilConstants.h"
#include "dxc/DXIL/DxilTypeSystem.h"

namespace llvm37 {
class Constant;
class Function;
class StructType;
class Type;
class Value;
} // namespace llvm37

namespace hlsl {
class DxilTypeSystem;

llvm37::StructType *GetLoweredUDT(
  llvm37::StructType *structTy, hlsl::DxilTypeSystem *pTypeSys = nullptr);
llvm37::Constant *TranslateInitForLoweredUDT(
    llvm37::Constant *Init, llvm37::Type *NewTy,
    // We need orientation for matrix fields
    hlsl::DxilTypeSystem *pTypeSys,
    hlsl::MatrixOrientation matOrientation = hlsl::MatrixOrientation::Undefined);
void ReplaceUsesForLoweredUDT(llvm37::Value *V, llvm37::Value *NewV);

} // namespace hlsl