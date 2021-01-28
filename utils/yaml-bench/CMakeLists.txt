add_llvm37_utility(yaml-bench
  YAMLBench.cpp
  )

target_link_libraries(yaml-bench LLVM37Support)
