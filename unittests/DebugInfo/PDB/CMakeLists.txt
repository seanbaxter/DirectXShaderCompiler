set(LLVM37_LINK_COMPONENTS
  DebugInfoPDB
  )

set(DebugInfoPDBSources
  PDBApiTest.cpp
  )

add_llvm37_unittest(DebugInfoPDBTests
  ${DebugInfoPDBSources}
  )
