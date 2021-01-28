list(APPEND LLVM37_COMMON_DEPENDS intrinsics_gen)

add_llvm_library(LLVM37Target
  Target.cpp
  TargetIntrinsicInfo.cpp
  TargetLoweringObjectFile.cpp
  TargetMachine.cpp
  TargetMachineC.cpp
  TargetRecip.cpp
  TargetSubtargetInfo.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Target
  )

foreach(t ${LLVM37_TARGETS_TO_BUILD})
  if (NOT t STREQUAL "None") # HLSL Change
  message(STATUS "Targeting ${t}")
  add_subdirectory(${t})
  endif() # HLSL Change
endforeach()
