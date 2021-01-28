set(LLVM37_LINK_COMPONENTS
  Object
  Support
  )

add_llvm37_tool(macho-dump
  macho-dump.cpp
  )
