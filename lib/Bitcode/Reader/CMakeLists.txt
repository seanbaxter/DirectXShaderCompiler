set(HLSL_IGNORE_SOURCES BitReader.cpp)

add_llvm_library(LLVM37BitReader
  # BitReader.cpp # HLSL Change - this is just the C wrapper
  BitcodeReader.cpp
  BitstreamReader.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Bitcode
  )

add_dependencies(LLVM37BitReader intrinsics_gen)
