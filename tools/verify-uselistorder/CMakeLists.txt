set(LLVM37_LINK_COMPONENTS
  AsmParser
  BitReader
  BitWriter
  Core
  IRReader
  Support
  )

add_llvm37_tool(verify-uselistorder
  verify-uselistorder.cpp
  )
