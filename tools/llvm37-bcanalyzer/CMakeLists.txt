set(LLVM37_LINK_COMPONENTS
  BitReader
  Support
  )

add_llvm37_tool(llvm37-bcanalyzer
  llvm37-bcanalyzer.cpp
  )
