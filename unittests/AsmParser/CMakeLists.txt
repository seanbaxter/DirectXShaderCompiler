set(LLVM37_LINK_COMPONENTS
  AsmParser
  Core
  Support
  )

add_llvm_unittest(AsmParserTests
  AsmParserTest.cpp
  )
