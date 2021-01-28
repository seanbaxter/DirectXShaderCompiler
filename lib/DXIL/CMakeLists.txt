# Copyright (C) Microsoft Corporation. All rights reserved.
# This file is distributed under the University of Illinois Open Source License. See LICENSE.TXT for details.
add_llvm_library(LLVM37DXIL
  DxilCBuffer.cpp
  DxilCompType.cpp
  DxilCounters.cpp
  DxilInterpolationMode.cpp
  DxilMetadataHelper.cpp
  DxilModule.cpp
  DxilOperations.cpp
  DxilResource.cpp
  DxilResourceBase.cpp
  DxilResourceBinding.cpp
  DxilResourceProperties.cpp
  DxilSampler.cpp
  DxilSemantic.cpp
  DxilShaderFlags.cpp
  DxilShaderModel.cpp
  DxilSignature.cpp
  DxilSignatureElement.cpp
  DxilSubobject.cpp
  DxilTypeSystem.cpp
  DxilUtil.cpp
  DxilPDB.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/IR
)

add_dependencies(LLVM37DXIL intrinsics_gen)
