add_llvm_library(LLVM37IRReader
  IRReader.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/IRReader

  DEPENDS
  intrinsics_gen
  )
