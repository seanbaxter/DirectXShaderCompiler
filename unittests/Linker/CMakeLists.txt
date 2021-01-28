set(LLVM37_LINK_COMPONENTS
  AsmParser
  core
  linker
  )

set(LinkerSources
  LinkModulesTest.cpp
  )

add_llvm37_unittest(LinkerTests
  ${LinkerSources}
  )
