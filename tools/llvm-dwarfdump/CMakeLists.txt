set(LLVM37_LINK_COMPONENTS
  DebugInfoDWARF
  Object
  Support
  )

add_llvm_tool(llvm-dwarfdump
  llvm-dwarfdump.cpp
  )

if(LLVM37_USE_SANITIZE_COVERAGE)
  add_subdirectory(fuzzer)
endif()
