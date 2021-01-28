///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilNoops.h                                                               //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#pragma once

#include "llvm37/ADT/StringRef.h"

namespace llvm37 {
  class Instruction;
}

namespace hlsl {
static const llvm37::StringRef kNoopName = "dx.noop";
static const llvm37::StringRef kPreservePrefix = "dx.preserve.";
static const llvm37::StringRef kNothingName = "dx.nothing.a";
static const llvm37::StringRef kPreserveName = "dx.preserve.value.a";

bool IsPreserve(llvm37::Instruction *S);
}
