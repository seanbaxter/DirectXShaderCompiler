add_llvm_library(LLVM37Instrumentation
  AddressSanitizer.cpp
  BoundsChecking.cpp
  DataFlowSanitizer.cpp
  GCOVProfiling.cpp
  MemorySanitizer.cpp
  Instrumentation.cpp
  InstrProfiling.cpp
  SafeStack.cpp
  SanitizerCoverage.cpp
  ThreadSanitizer.cpp

  ADDITIONAL_HEADER_DIRS
  ${LLVM37_MAIN_INCLUDE_DIR}/llvm37/Transforms
  )

add_dependencies(LLVM37Instrumentation intrinsics_gen)
