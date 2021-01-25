set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  LTO
  MC
  Support
  Target
  )

add_llvm_tool(llvm-lto
  llvm-lto.cpp
  )

