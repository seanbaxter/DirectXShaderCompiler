# AsmParser
add_llvm_library(LLVM37AsmParser
  LLLexer.cpp
  LLParser.cpp
  Parser.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Analysis

  DEPENDS
  intrinsics_gen
  )
