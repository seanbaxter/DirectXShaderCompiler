set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  Object
  Support
  )

add_llvm37_tool(llvm37-cxxdump
  llvm37-cxxdump.cpp
  Error.cpp
  )
