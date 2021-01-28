//===-- DxilConstantFolding.h - Constant folding for Dxil ------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//===----------------------------------------------------------------------===//
//
// This file declares routines for folding dxil intrinsics into constants when
// all operands are constants.
//
// We hook into the LLVM37 routines for constant folding so the function
// interfaces are dictated by what llvm37 provides.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_ANALYSIS_HLSLCONSTANTFOLDING_H
#define LLVM37_ANALYSIS_HLSLCONSTANTFOLDING_H
#include "llvm37/ADT/StringRef.h"

namespace llvm37 {
  class Constant;
  class Function;
  class Type;
  template<typename T>
  class ArrayRef;
}

namespace hlsl {
  /// ConstantFoldScalarCall - Try to constant fold the call instruction.
  /// If successful, the constant result is returned, if not, null is returned.
  llvm37::Constant *ConstantFoldScalarCall(llvm37::StringRef Name, llvm37::Type *Ty, llvm37::ArrayRef<llvm37::Constant *> Operands);

  /// ConstantFoldScalarCallExt
  /// Hook point for constant folding of extensions.
  llvm37::Constant *ConstantFoldScalarCallExt(llvm37::StringRef Name, llvm37::Type *Ty, llvm37::ArrayRef<llvm37::Constant *> Operands);

  /// CanConstantFoldCallTo - Return true if we can potentially constant
  /// fold a call to the given function.
  bool CanConstantFoldCallTo(const llvm37::Function *F);

  /// CanConstantFoldCallToExt
  /// Hook point for constant folding of extensions.
  bool CanConstantFoldCallToExt(const llvm37::Function *F);
}

#endif
