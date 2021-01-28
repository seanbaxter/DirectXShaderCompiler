if( LLVM37_USE_SANITIZE_COVERAGE )
  set(LLVM37_LINK_COMPONENTS support)

  add_clang_executable(clang-fuzzer
    EXCLUDE_FROM_ALL
    ClangFuzzer.cpp
    )

  target_link_libraries(clang-fuzzer
    ${CLANG_FORMAT_LIB_DEPS}
    clangAST
    clangBasic
    clangDriver
    clangFrontend
    clangRewriteFrontend
    clangStaticAnalyzerFrontend
    clangTooling
    LLVM37Fuzzer
    )
endif()
