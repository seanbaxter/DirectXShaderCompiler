set(LLVM37_LINK_COMPONENTS
  IPA
  Analysis
  AsmParser
  Core
  Support
  )

add_llvm37_unittest(AnalysisTests
  AliasAnalysisTest.cpp
  CallGraphTest.cpp
  CFGTest.cpp
  LazyCallGraphTest.cpp
  ScalarEvolutionTest.cpp
  MixedTBAATest.cpp
  )
