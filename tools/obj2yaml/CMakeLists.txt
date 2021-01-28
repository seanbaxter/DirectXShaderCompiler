set(LLVM37_LINK_COMPONENTS
  Object
  Support
  )

add_llvm37_tool(obj2yaml
  obj2yaml.cpp coff2yaml.cpp elf2yaml.cpp Error.cpp
  )
