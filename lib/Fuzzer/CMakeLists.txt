set(LIBFUZZER_FLAGS_BASE "${CMAKE_CXX_FLAGS_RELEASE}")
# Disable the coverage and sanitizer instrumentation for the fuzzer itself.
set(CMAKE_CXX_FLAGS_RELEASE "${LIBFUZZER_FLAGS_BASE} -O2 -fno-sanitize=all")
if( LLVM37_USE_SANITIZE_COVERAGE )
  add_library(LLVM37FuzzerNoMainObjects OBJECT
    FuzzerCrossOver.cpp
    FuzzerInterface.cpp
    FuzzerTraceState.cpp
    FuzzerDriver.cpp
    FuzzerIO.cpp
    FuzzerLoop.cpp
    FuzzerMutate.cpp
    FuzzerSanitizerOptions.cpp
    FuzzerSHA1.cpp
    FuzzerUtil.cpp
    )
  add_library(LLVM37FuzzerNoMain STATIC
    $<TARGET_OBJECTS:LLVM37FuzzerNoMainObjects>
    )
  add_library(LLVM37Fuzzer STATIC
    FuzzerMain.cpp
    $<TARGET_OBJECTS:LLVM37FuzzerNoMainObjects>
    )

  if( LLVM37_INCLUDE_TESTS )
    add_subdirectory(test)
  endif()
endif()
