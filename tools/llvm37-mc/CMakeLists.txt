set(LLVM37_LINK_COMPONENTS
  AllTargetsAsmPrinters
  AllTargetsAsmParsers
  AllTargetsDescs
  AllTargetsDisassemblers
  AllTargetsInfos
  MC
  MCParser
  Support
  )

add_llvm37_tool(llvm37-mc
  llvm37-mc.cpp
  Disassembler.cpp
  )
