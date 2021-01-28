set(LLVM37_LINK_COMPONENTS
  DebugInfoDWARF
  Object
  Support
  )

add_llvm37_tool(llvm37-dwarfdump
  llvm37-dwarfdump.cpp
  )

if(LLVM37_USE_SANITIZE_COVERAGE)
  add_subdirectory(fuzzer)
endif()
