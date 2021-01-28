add_llvm_library(LLVM37Passes
  PassBuilder.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Passes
  )

add_dependencies(LLVM37Passes intrinsics_gen)
