set(LLVM37_LINK_COMPONENTS
  BitReader
  Support
  )

add_llvm_tool(llvm-bcanalyzer
  llvm-bcanalyzer.cpp
  )
