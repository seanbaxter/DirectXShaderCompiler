//===-- DxilSimplify.h - Simplify Dxil operations ------*- C++ -*-===//
//
//                     The LLVM37 Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
// Copyright (C) Microsoft Corporation. All rights reserved.
//===----------------------------------------------------------------------===//
//
// This file declares routines for simplify dxil intrinsics when some operands
// are constants.
//
// We hook into the llvm37::SimplifyInstruction so the function
// interfaces are dictated by what llvm37 provides.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM37_ANALYSIS_HLSLDXILSIMPLIFY_H
#define LLVM37_ANALYSIS_HLSLDXILSIMPLIFY_H
#include "llvm37/ADT/ArrayRef.h"

namespace llvm37 {
class Function;
class Instruction;
class Value;
} // namespace llvm37

namespace hlsl {

/// \brief Given a function and set of arguments, see if we can fold the
/// result as dxil operation.
///
/// If this call could not be simplified returns null.
llvm37::Value *SimplifyDxilCall(llvm37::Function *F,
                              llvm37::ArrayRef<llvm37::Value *> Args,
                              llvm37::Instruction *I,
                              bool MayInsert);

/// CanSimplify
/// Return true on dxil operation function which can be simplified.
bool CanSimplify(const llvm37::Function *F);
} // namespace hlsl

#endif
