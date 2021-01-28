set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  CodeGen
  DebugInfoDWARF
  MC
  MCDisassembler
  Object
  Support
  )

add_llvm37_tool(llvm37-objdump
  llvm37-objdump.cpp
  COFFDump.cpp
  ELFDump.cpp
  MachODump.cpp
  )
