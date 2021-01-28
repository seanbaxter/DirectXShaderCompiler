set(LLVM37_LINK_COMPONENTS
  Object
  Support
  )

add_llvm37_tool(llvm37-readobj
  ARMAttributeParser.cpp
  ARMWinEHPrinter.cpp
  COFFDumper.cpp
  ELFDumper.cpp
  Error.cpp
  llvm37-readobj.cpp
  MachODumper.cpp
  ObjDumper.cpp
  StreamWriter.cpp
  Win64EHDumper.cpp
  )
