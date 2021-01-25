set(LLVM37_LINK_COMPONENTS
  AsmParser
  core
  linker
  )

set(LinkerSources
  LinkModulesTest.cpp
  )

add_llvm_unittest(LinkerTests
  ${LinkerSources}
  )
