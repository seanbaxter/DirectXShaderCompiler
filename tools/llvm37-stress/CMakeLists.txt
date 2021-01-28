set(LLVM37_LINK_COMPONENTS
  Core
  IPA
  Support
  )

add_llvm37_tool(llvm37-stress
  llvm37-stress.cpp
  )
export_executable_symbols(llvm37-stress)
