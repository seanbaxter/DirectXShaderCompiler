set(LLVM37_LINK_COMPONENTS
  DebugInfoDWARF
  )

set(DebugInfoSources
  DWARFFormValueTest.cpp
  )

add_llvm37_unittest(DebugInfoDWARFTests
  ${DebugInfoSources}
  )
