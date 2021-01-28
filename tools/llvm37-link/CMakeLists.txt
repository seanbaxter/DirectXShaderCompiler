set(LLVM37_LINK_COMPONENTS
  BitWriter
  Core
  IRReader
  Linker
  Support
  )

add_llvm37_tool(llvm37-link
  llvm37-link.cpp
  )
