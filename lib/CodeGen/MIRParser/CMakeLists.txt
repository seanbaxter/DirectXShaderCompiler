add_llvm_library(LLVM37MIRParser
  MILexer.cpp
  MIParser.cpp
  MIRParser.cpp
  )

add_dependencies(LLVM37MIRParser intrinsics_gen)
