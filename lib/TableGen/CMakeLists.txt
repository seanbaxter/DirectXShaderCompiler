add_llvm_library(LLVM37TableGen
  Error.cpp
  Main.cpp
  Record.cpp
  SetTheory.cpp
  StringMatcher.cpp
  TableGenBackend.cpp
  TGLexer.cpp
  TGParser.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/TableGen
  )
