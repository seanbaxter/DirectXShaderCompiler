add_llvm_library(LLVM37LTO
  LTOModule.cpp
  LTOCodeGenerator.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/LTO
  )

add_dependencies(LLVM37LTO intrinsics_gen)
