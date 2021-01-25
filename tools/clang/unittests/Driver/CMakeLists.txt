set(LLVM37_LINK_COMPONENTS
  Support
  )

add_clang_unittest(ClangDriverTests
  MultilibTest.cpp
  )

target_link_libraries(ClangDriverTests
  clangDriver
  )
