set(LLVM37_LINK_COMPONENTS
  AsmPrinter
  Support
  )

set(CodeGenSources
  DIEHashTest.cpp
  )

add_llvm_unittest(CodeGenTests
  ${CodeGenSources}
  )
