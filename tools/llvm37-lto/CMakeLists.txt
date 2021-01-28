set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  LTO
  MC
  Support
  Target
  )

add_llvm37_tool(llvm37-lto
  llvm37-lto.cpp
  )

