///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// HLMatrixLowerHelper.h                                                     //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
// This file provides helper functions to lower high level matrix.           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "llvm37/IR/IRBuilder.h"

namespace llvm37 {
  class Type;
  class Value;
  template<typename T>
  class ArrayRef;
}

namespace hlsl {

class DxilFieldAnnotation;
class DxilTypeSystem;

namespace HLMatrixLower {

llvm37::Value *BuildVector(llvm37::Type *EltTy,
                         llvm37::ArrayRef<llvm37::Value *> elts,
                         llvm37::IRBuilder<> &Builder);

} // namespace HLMatrixLower

} // namespace hlsl