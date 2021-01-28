
include_directories( ${LLVM37_OPROFILE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/.. )

add_llvm_library(LLVM37OProfileJIT
  OProfileJITEventListener.cpp
  OProfileWrapper.cpp
  )
