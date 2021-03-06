set(LLVM37_LINK_COMPONENTS support
  mcparser # HLSL Change - needed for 'no target' build
  hlsl  # HLSL Change
)

add_clang_executable(clang-format
  ClangFormat.cpp
  )

set(CLANG_FORMAT_LIB_DEPS
  clangBasic
  clangFormat
  clangRewrite
  clangToolingCore
  )

target_link_libraries(clang-format
  ${CLANG_FORMAT_LIB_DEPS}
  )

if( LLVM37_USE_SANITIZE_COVERAGE )
  add_subdirectory(fuzzer)
endif()

install(TARGETS clang-format RUNTIME DESTINATION bin)
install(PROGRAMS clang-format-bbedit.applescript DESTINATION share/clang)
install(PROGRAMS clang-format-diff.py DESTINATION share/clang)
install(PROGRAMS clang-format-sublime.py DESTINATION share/clang)
install(PROGRAMS clang-format.el DESTINATION share/clang)
install(PROGRAMS clang-format.py DESTINATION share/clang)
install(PROGRAMS git-clang-format DESTINATION bin)
