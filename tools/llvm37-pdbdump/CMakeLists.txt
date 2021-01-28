set(LLVM37_LINK_COMPONENTS
  Support
  DebugInfoPDB
  )

add_llvm37_tool(llvm37-pdbdump
  llvm37-pdbdump.cpp
  BuiltinDumper.cpp
  ClassDefinitionDumper.cpp
  CompilandDumper.cpp
  EnumDumper.cpp
  ExternalSymbolDumper.cpp
  FunctionDumper.cpp
  LinePrinter.cpp
  TypeDumper.cpp
  TypedefDumper.cpp
  VariableDumper.cpp
  )
