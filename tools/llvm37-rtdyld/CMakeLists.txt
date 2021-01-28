set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  DebugInfoDWARF
  ExecutionEngine
  MC
  Object
  RuntimeDyld
  Support
  )

add_llvm37_tool(llvm37-rtdyld
  llvm37-rtdyld.cpp
  )
