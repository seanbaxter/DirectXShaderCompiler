set(LLVM37_LINK_COMPONENTS
  Core
  Support
  )

add_clang_unittest(ClangCodeGenTests
  BufferSourceTest.cpp
  )

target_link_libraries(ClangCodeGenTests
  clangBasic
  clangCodeGen
  clangFrontend
  clangParse
  )
