set(LLVM37_LINK_COMPONENTS support)

add_clang_library(clangToolingCore
  Replacement.cpp

  LINK_LIBS
  clangBasic
  clangLex
  clangRewrite
  )
