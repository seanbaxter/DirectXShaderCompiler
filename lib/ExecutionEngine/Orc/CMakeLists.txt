add_llvm_library(LLVM37OrcJIT
  ExecutionUtils.cpp
  IndirectionUtils.cpp
  NullResolver.cpp
  OrcMCJITReplacement.cpp
  OrcTargetSupport.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/ExecutionEngine/Orc

  DEPENDS
  intrinsics_gen
  )
