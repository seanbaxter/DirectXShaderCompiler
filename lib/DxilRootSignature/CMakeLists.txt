# Copyright (C) Microsoft Corporation. All rights reserved.
# This file is distributed under the University of Illinois Open Source License. See LICENSE.TXT for details.
add_llvm_library(LLVM37DxilRootSignature
  DxilRootSignature.cpp
  DxilRootSignatureConvert.cpp
  DxilRootSignatureSerializer.cpp
  DxilRootSignatureValidator.cpp


  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/IR
)

add_dependencies(LLVM37DxilRootSignature intrinsics_gen)
