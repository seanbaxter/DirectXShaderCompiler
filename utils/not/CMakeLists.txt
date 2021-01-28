add_llvm37_utility(not
  not.cpp
  )

target_link_libraries(not LLVM37Support LLVM37MSSupport) # HLSL Change
