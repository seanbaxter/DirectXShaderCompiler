set(LLVM37_LINK_COMPONENTS
  DebugInfoDWARF
  )

set(DebugInfoSources
  DWARFFormValueTest.cpp
  )

add_llvm_unittest(DebugInfoDWARFTests
  ${DebugInfoSources}
  )
