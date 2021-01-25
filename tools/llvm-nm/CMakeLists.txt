set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  Core
  Object
  Support
  )

add_llvm_tool(llvm-nm
  llvm-nm.cpp
  )
