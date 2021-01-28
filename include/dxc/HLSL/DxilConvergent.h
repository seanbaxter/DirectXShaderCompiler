///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// DxilConvergent.h                                                          //
// Copyright (C) Microsoft Corporation. All rights reserved.                 //
// This file is distributed under the University of Illinois Open Source     //
// License. See LICENSE.TXT for details.                                     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
#pragma once

namespace llvm37 {
  class Value;
  class Function;
}

namespace hlsl {
  bool IsConvergentMarker(llvm37::Value *V);
  llvm37::Value *GetConvergentSource(llvm37::Value *V);
}
