set(LLVM37_LINK_COMPONENTS
  BitWriter
  Core
  IRReader
  Linker
  Support
  )

add_llvm_tool(llvm-link
  llvm-link.cpp
  )
