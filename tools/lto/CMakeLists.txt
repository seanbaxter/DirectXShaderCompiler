set(LLVM37_LINK_COMPONENTS
  ${LLVM37_TARGETS_TO_BUILD}
  Core
  LTO
  MC
  MCDisassembler
  Support
  Target
  )

set(SOURCES
  LTODisassembler.cpp
  lto.cpp
  )

set(LLVM37_EXPORTED_SYMBOL_FILE ${CMAKE_CURRENT_SOURCE_DIR}/lto.exports)

add_llvm_library(LTO SHARED ${SOURCES})

install(FILES ${LLVM37_MAIN_INCLUDE_DIR}/llvm37-c/lto.h
  DESTINATION include/llvm37-c)
