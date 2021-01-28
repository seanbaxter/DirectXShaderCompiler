set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  Core
  Object
  Support
  )

add_llvm37_tool(llvm37-nm
  llvm37-nm.cpp
  )
