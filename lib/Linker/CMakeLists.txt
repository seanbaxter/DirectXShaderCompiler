add_llvm_library(LLVM37Linker
  LinkModules.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Linker

  DEPENDS
  intrinsics_gen
  )
