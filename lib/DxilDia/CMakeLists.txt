# Copyright (C) Microsoft Corporation. All rights reserved.
# This file is distributed under the University of Illinois Open Source License. See LICENSE.TXT for details.

if (WIN32)
  find_package(DiaSDK REQUIRED) # Used for constants and declarations.
endif (WIN32)

add_llvm_library(LLVM37DxilDia
  DxcPixCompilationInfo.cpp
  DxcPixDxilDebugInfo.cpp
  DxcPixDxilStorage.cpp
  DxcPixEntrypoints.cpp
  DxcPixLiveVariables.cpp
  DxcPixLiveVariables_FragmentIterator.cpp
  DxcPixTypes.cpp
  DxcPixVariables.cpp
  DxilDia.cpp
  DxilDiaDataSource.cpp
  DxilDiaEnumTables.cpp
  DxilDiaSession.cpp
  DxilDiaSymbolManager.cpp
  DxilDiaTable.cpp
  DxilDiaTableFrameData.cpp
  DxilDiaTableInjectedSources.cpp
  DxilDiaTableInputAssemblyFile.cpp
  DxilDiaTableLineNumbers.cpp
  DxilDiaTableSections.cpp
  DxilDiaTableSegmentMap.cpp
  DxilDiaTableSourceFiles.cpp
  DxilDiaTableSymbols.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/IR
)

if (WIN32)
  target_link_libraries(LLVM37DxilDia PRIVATE ${LIBRARIES} ${DIASDK_LIBRARIES})
  include_directories(AFTER ${LLVM37_INCLUDE_DIR}/dxc/Tracing ${DIASDK_INCLUDE_DIRS})
endif (WIN32)

add_dependencies(LLVM37DxilDia LLVM37DxilPIXPasses intrinsics_gen)
