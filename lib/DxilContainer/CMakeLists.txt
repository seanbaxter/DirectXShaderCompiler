# Copyright (C) Microsoft Corporation. All rights reserved.
# This file is distributed under the University of Illinois Open Source License. See LICENSE.TXT for details.
add_llvm_library(LLVM37DxilContainer
  DxilContainer.cpp
  DxilContainerAssembler.cpp
  DxilContainerReader.cpp
  DxcContainerBuilder.cpp
  DxilRuntimeReflection.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/IR
)

add_dependencies(LLVM37DxilContainer intrinsics_gen)
