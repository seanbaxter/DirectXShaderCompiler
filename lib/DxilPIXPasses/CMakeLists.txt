# Copyright (C) Microsoft Corporation. All rights reserved.
# This file is distributed under the University of Illinois Open Source License. See LICENSE.TXT for details.
add_llvm_library(LLVM37DxilPIXPasses
  DxilAddPixelHitInstrumentation.cpp
  DxilAnnotateWithVirtualRegister.cpp
  DxilDbgValueToDbgDeclare.cpp
  DxilDebugInstrumentation.cpp
  DxilForceEarlyZ.cpp
  DxilOutputColorBecomesConstant.cpp
  DxilPIXMeshShaderOutputInstrumentation.cpp
  DxilRemoveDiscards.cpp
  DxilReduceMSAAToSingleSample.cpp
  DxilShaderAccessTracking.cpp
  DxilPIXPasses.cpp
  DxilPIXVirtualRegisters.cpp


  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/IR
)

add_dependencies(LLVM37DxilPIXPasses intrinsics_gen)
