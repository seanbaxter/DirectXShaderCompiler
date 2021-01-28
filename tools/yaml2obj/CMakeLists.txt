set(LLVM37_LINK_COMPONENTS
  MC
  Object
  Support
  )

add_llvm37_tool(yaml2obj
  yaml2obj.cpp
  yaml2coff.cpp
  yaml2elf.cpp
  )
