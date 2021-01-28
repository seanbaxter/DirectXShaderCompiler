set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  AsmPrinter
  DebugInfoDWARF
  MC
  Object
  Support
  Target
  )

add_llvm37_tool(llvm37-dsymutil
  dsymutil.cpp
  BinaryHolder.cpp
  DebugMap.cpp
  DwarfLinker.cpp
  MachODebugMapParser.cpp
  )

