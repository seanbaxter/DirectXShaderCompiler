set(LLVM37_LINK_COMPONENTS
  Option
  Support
  )

set(LLVM37_TARGET_DEFINITIONS Opts.td)

tablegen(LLVM37 Opts.inc -gen-opt-parser-defs)
add_public_tablegen_target(OptsTestTableGen)

add_llvm37_unittest(OptionTests
  OptionParsingTest.cpp
  )
