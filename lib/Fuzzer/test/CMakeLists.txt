# Build all these tests with -O0, otherwise optimizations may merge some
# basic blocks and we'll fail to discover the targets.
# Also enable the coverage instrumentation back (it is disabled
# for the Fuzzer lib)
set(CMAKE_CXX_FLAGS_RELEASE "${LIBFUZZER_FLAGS_BASE} -O0 -fsanitize-coverage=edge,indirect-calls")

set(DFSanTests
  DFSanMemcmpTest
  DFSanSimpleCmpTest
  )

set(Tests
  CounterTest
  CxxTokensTest
  FourIndependentBranchesTest
  FullCoverageSetTest
  InfiniteTest
  NullDerefTest
  SimpleTest
  TimeoutTest
  ${DFSanTests}
  )

set(CustomMainTests
  UserSuppliedFuzzerTest
  )


set(TestBinaries)

foreach(Test ${Tests})
  add_executable(LLVM37Fuzzer-${Test}
    ${Test}.cpp
    )
  target_link_libraries(LLVM37Fuzzer-${Test}
    LLVM37Fuzzer
    )
  set(TestBinaries ${TestBinaries} LLVM37Fuzzer-${Test})
endforeach()

foreach(Test ${CustomMainTests})
  add_executable(LLVM37Fuzzer-${Test}
    ${Test}.cpp
    )
  target_link_libraries(LLVM37Fuzzer-${Test}
    LLVM37FuzzerNoMain
    )
  set(TestBinaries ${TestBinaries} LLVM37Fuzzer-${Test})
endforeach()


configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
  )

configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/unit/lit.site.cfg.in
  ${CMAKE_CURRENT_BINARY_DIR}/unit/lit.site.cfg
  )

include_directories(..)
include_directories(${LLVM37_MAIN_SRC_DIR}/utils/unittest/googletest/include)

add_executable(LLVM37Fuzzer-Unittest
  FuzzerUnittest.cpp
  $<TARGET_OBJECTS:LLVM37FuzzerNoMainObjects>
  )

target_link_libraries(LLVM37Fuzzer-Unittest
  gtest
  gtest_main
  )

set(TestBinaries ${TestBinaries} LLVM37Fuzzer-Unittest)

add_subdirectory(dfsan)

foreach(Test ${DFSanTests})
  set(TestBinaries ${TestBinaries} LLVM37Fuzzer-${Test}-DFSan)
endforeach()


set_target_properties(${TestBinaries}
  PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  )

add_lit_testsuite(check-fuzzer "Running Fuzzer tests"
    ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${TestBinaries} FileCheck not
    )
