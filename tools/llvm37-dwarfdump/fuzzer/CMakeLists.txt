set(LLVM37_LINK_COMPONENTS
  DebugInfoDWARF
  Object
  Support
  )

add_llvm37_executable(llvm37-dwarfdump-fuzzer
  EXCLUDE_FROM_ALL
  llvm37-dwarfdump-fuzzer.cpp
  )

target_link_libraries(llvm37-dwarfdump-fuzzer
  LLVM37Fuzzer
  )
