add_llvm_library(LLVM37Option
  Arg.cpp
  ArgList.cpp
  Option.cpp
  OptTable.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Option
  )
