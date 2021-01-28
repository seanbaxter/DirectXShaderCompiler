set(LLVM37_LINK_COMPONENTS
  BitWriter
  Core
  IPO
  IRReader
  Support
  )

add_llvm37_tool(llvm37-extract
  llvm37-extract.cpp
  )
