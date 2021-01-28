set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  MC
  MCDisassembler
  Support
  )

add_llvm37_unittest(MCTests
  Disassembler.cpp
  StringTableBuilderTest.cpp
  YAMLTest.cpp
  )
