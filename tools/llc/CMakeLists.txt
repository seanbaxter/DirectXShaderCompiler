set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  Analysis
  AsmPrinter
  CodeGen
  Core
  IRReader
  MC
  MIRParser
  ScalarOpts
  SelectionDAG
  Support
  Target
  )

# Support plugins.
set(LLVM37_NO_DEAD_STRIP 1)

add_llvm37_tool(llc
  llc.cpp
  )
export_executable_symbols(llc)
