add_llvm_library(LLVM37MCJIT
  MCJIT.cpp

  DEPENDS
  intrinsics_gen
  )
