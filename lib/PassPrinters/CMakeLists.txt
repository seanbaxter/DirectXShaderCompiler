add_llvm_library(LLVM37PassPrinters
  PassPrinters.cpp
  ADDITIONAL_HEADER_DIRS
)

add_dependencies(LLVM37PassPrinters intrinsics_gen)
